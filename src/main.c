#include "controller.h"
#include "controllers/dinput_controller.h"
#include "controllers/ds3_controller.h"
#include "controllers/xbox_360_controller.h"
#include "controllers/xbox_360w_controller.h"
#include "controllers/xbox_controller.h"
#include "devicelist.h"

#include <psp2kern/ctrl.h>
#include <psp2kern/kernel/cpu.h>
#include <psp2kern/kernel/debug.h>
#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/kernel/suspend.h>
#include <psp2kern/kernel/sysclib.h>
#include <psp2kern/kernel/sysmem/data_transfers.h>
#include <psp2kern/kernel/threadmgr.h>
#include <psp2kern/kernel/threadmgr/event_flags.h>
#include <psp2kern/kernel/aimgr.h>
#include <psp2kern/usbd.h>
#include <psp2kern/usbserv.h>
#include <taihen.h>

#define MAX_CONTROLLERS 4

#define DECL_FUNC_HOOK(name, ...)                                                                                      \
  static tai_hook_ref_t name##HookRef;                                                                                 \
  static SceUID name##HookUid = -1;                                                                                    \
  static int name##HookFunc(__VA_ARGS__)

#define BIND_FUNC_OFFSET_HOOK(name, pid, modid, segidx, offset, thumb)                                                 \
  name##HookUid = taiHookFunctionOffsetForKernel((pid), &name##HookRef, (modid), (segidx), (offset), thumb,            \
                                                 (const void *)name##HookFunc)

#define BIND_FUNC_EXPORT_HOOK(name, pid, module, lib_nid, func_nid)                                                    \
  name##HookUid = taiHookFunctionExportForKernel((pid), &name##HookRef, (module), (lib_nid), (func_nid),               \
                                                 (const void *)name##HookFunc)

#define UNBIND_FUNC_HOOK(name)                                                                                         \
  ({                                                                                                                   \
    if (name##HookUid > 0)                                                                                             \
      taiHookReleaseForKernel(name##HookUid, name##HookRef);                                                           \
  })

static int started = 0;

static Controller controllers[MAX_CONTROLLERS];

/*
 * ViXEn USB slot and PSTV controller port are not the same thing.
 *
 * Native DS3/DS4 controllers own their PSTV ports. ViXEn controllers
 * are assigned to the remaining free ports in USB-slot order.
 *
 * 0 = no Vita port assigned
 * 1..4 = PSTV controller port
 */
static int vixen_vita_port[MAX_CONTROLLERS] = {0, 0, 0, 0};
static uint8_t raw_port_type[MAX_CONTROLLERS + 1] = {0, 0, 0, 0, 0};

static int is_native_controller_type(uint8_t type)
{
  return type == SCE_CTRL_TYPE_DS3 || type == SCE_CTRL_TYPE_DS4;
}

static int vixen_slot_for_vita_port(int port)
{
  if (port < 1 || port > MAX_CONTROLLERS)
    return -1;

  for (int i = 0; i < MAX_CONTROLLERS; i++)
  {
    if (controllers[i].attached &&
        controllers[i].inited &&
        vixen_vita_port[i] == port)
      return i;
  }

  return -1;
}

static int vixen_primary_slot(void)
{
  /*
   * On PSTV, port 0 is the primary/system controller path.
   * Do not inject a ViXEn controller into it while a native Sony
   * controller owns physical port 1.
   */
  if (is_native_controller_type(raw_port_type[1]))
    return -1;

  return vixen_slot_for_vita_port(1);
}

static void rebuild_vixen_port_map(void)
{
  int used[MAX_CONTROLLERS + 1] = {0, 0, 0, 0, 0};
  int new_map[MAX_CONTROLLERS] = {0, 0, 0, 0};

  /* Native DS3/DS4 ports always win. */
  for (int port = 1; port <= MAX_CONTROLLERS; port++)
  {
    if (is_native_controller_type(raw_port_type[port]))
      used[port] = 1;
  }

  /*
   * Deterministically assign active ViXEn controllers to the lowest
   * free PSTV ports in internal controller-slot order.
   */
  for (int slot = 0; slot < MAX_CONTROLLERS; slot++)
  {
    if (!controllers[slot].attached || !controllers[slot].inited)
      continue;

    for (int port = 1; port <= MAX_CONTROLLERS; port++)
    {
      if (!used[port])
      {
        new_map[slot] = port;
        used[port] = 1;
        break;
      }
    }
  }

  for (int slot = 0; slot < MAX_CONTROLLERS; slot++)
  {
    vixen_vita_port[slot] = new_map[slot];
  }
}

static inline int clamp(int value, int min, int max)
{
  if (value <= min)
    return min;
  if (value >= max)
    return max;
  return value;
}

DECL_FUNC_HOOK(ksceCtrlGetControllerPortInfo, SceCtrlPortInfo *info)
{
  int ret = TAI_CONTINUE(int, ksceCtrlGetControllerPortInfoHookRef, info);

  if (ret >= 0)
  {
    /*
     * Save the real PSTV controller topology BEFORE ViXEn spoofs its
     * own ports. This is authoritative for native Sony controllers.
     */
    for (int port = 0; port <= MAX_CONTROLLERS; port++)
    {
      raw_port_type[port] = info->port[port];
    }

    rebuild_vixen_port_map();

    /*
     * Advertise each assigned ViXEn controller as DS3, but never
     * overwrite a real DS3/DS4 controller reported by the PSTV.
     */
    for (int slot = 0; slot < MAX_CONTROLLERS; slot++)
    {
      int port = vixen_vita_port[slot];

      if (controllers[slot].inited &&
          controllers[slot].attached &&
          port >= 1 &&
          port <= MAX_CONTROLLERS &&
          !is_native_controller_type(raw_port_type[port]))
      {
        info->port[port] = SCE_CTRL_TYPE_DS3;
      }
    }

  }

  return ret;
}

DECL_FUNC_HOOK(sceCtrlGetBatteryInfo, int port, uint8_t *batt)
{
  int cont = vixen_slot_for_vita_port(port);

  if (cont >= 0 &&
      controllers[cont].type == PAD_XBOX360W)
  {
    uint8_t data;
    ksceKernelMemcpyUserToKernel(&data, (void *)batt, sizeof(uint8_t));
    data = controllers[cont].battery_level;
    ksceKernelMemcpyKernelToUser((void *)batt, &data, sizeof(uint8_t));
    return 0;
  }

  return TAI_CONTINUE(int, sceCtrlGetBatteryInfoHookRef, port, batt);
}

DECL_FUNC_HOOK(sceCtrlSetActuator, int port, const SceCtrlActuator *pState)
{
  int cont = vixen_slot_for_vita_port(port);

  if (cont >= 0)
  {
    SceCtrlActuator lpState;
    ksceKernelMemcpyUserToKernel(&lpState, (void *)pState, sizeof(SceCtrlActuator));

    switch (controllers[cont].type)
    {
      case PAD_XBOX360:
        Xbox360Controller_setRumble(&controllers[cont], lpState.small, lpState.large);
        break;
      case PAD_DS3:
        DS3Controller_setRumble(&controllers[cont], lpState.small, lpState.large);
        break;
      case PAD_XBOX360W:
        Xbox360WController_setRumble(&controllers[cont], lpState.small, lpState.large);
        break;
      case PAD_XBOX:
        XboxController_setRumble(&controllers[cont], lpState.small, lpState.large);
        break;
      default:
        break;
    }

    return 0;
  }

  return TAI_CONTINUE(int, sceCtrlSetActuatorHookRef, port, pState);
}

DECL_FUNC_HOOK(sceCtrlDisconnect, int port)
{
  int cont = vixen_slot_for_vita_port(port);

  if (cont >= 0 && controllers[cont].type == PAD_XBOX360W)
  {
    Xbox360WController_turnOff(&controllers[cont]);
    return 0;
  }

  return TAI_CONTINUE(int, sceCtrlDisconnectHookRef, port);
}

static void patchControlData(int port, SceCtrlData *data, int count, uint8_t negative, uint8_t triggers_ext)
{
  int cont;

  if (port == 0)
    cont = vixen_primary_slot();
  else
    cont = vixen_slot_for_vita_port(port);

  if (cont < 0)
    return;

  if (!controllers[cont].inited || !controllers[cont].attached)
    return;

  ControlData *controlData = &(controllers[cont].controlData);

  if (port == 0)
  {
    uint32_t buttons = controlData->buttons;
    ksceCtrlSetButtonEmulation(0, 0, buttons, buttons, 16);
  }

  for (int i = 0; i < count; i++)
  {
    if (port > 0)
    {
      if (negative)
        data[i].buttons &= ~controlData->buttons;
      else
        data[i].buttons |= controlData->buttons;
    }

    data[i].lt = clamp(data[i].lt + controlData->lt, 0, 255);
    data[i].rt = clamp(data[i].rt + controlData->rt, 0, 255);

    data[i].lx = clamp(data[i].lx + controlData->leftX - 127, 0, 255);
    data[i].ly = clamp(data[i].ly + controlData->leftY - 127, 0, 255);
    data[i].rx = clamp(data[i].rx + controlData->rightX - 127, 0, 255);
    data[i].ry = clamp(data[i].ry + controlData->rightY - 127, 0, 255);
  }
}
#define DECL_FUNC_HOOK_CTRL(name, negative, triggers)                                                                  \
  DECL_FUNC_HOOK(name, int port, SceCtrlData *data, int count)                                                         \
  {                                                                                                                    \
    int ret = TAI_CONTINUE(int, name##HookRef, port, data, count);                                                     \
    if (ret >= 0)                                                                                                      \
      patchControlData(port, data, count, (negative), (triggers));                                                     \
    return ret;                                                                                                        \
  }

DECL_FUNC_HOOK_CTRL(ksceCtrlPeekBufferPositive, 0, 0)
DECL_FUNC_HOOK_CTRL(ksceCtrlReadBufferPositive, 0, 0)
DECL_FUNC_HOOK_CTRL(ksceCtrlPeekBufferNegative, 1, 0)
DECL_FUNC_HOOK_CTRL(ksceCtrlReadBufferNegative, 1, 0)
DECL_FUNC_HOOK_CTRL(ksceCtrlPeekBufferPositiveExt, 0, 0)
DECL_FUNC_HOOK_CTRL(ksceCtrlReadBufferPositiveExt, 0, 0)

DECL_FUNC_HOOK_CTRL(ksceCtrlPeekBufferPositive2, 0, 1)
DECL_FUNC_HOOK_CTRL(ksceCtrlReadBufferPositive2, 0, 1)
DECL_FUNC_HOOK_CTRL(ksceCtrlPeekBufferNegative2, 1, 1)
DECL_FUNC_HOOK_CTRL(ksceCtrlReadBufferNegative2, 1, 1)
DECL_FUNC_HOOK_CTRL(ksceCtrlPeekBufferPositiveExt2, 0, 1)
DECL_FUNC_HOOK_CTRL(ksceCtrlReadBufferPositiveExt2, 0, 1)

int libvixen_probe(int device_id);
int libvixen_attach(int device_id);
int libvixen_detach(int device_id);

static const SceUsbdDriver libvixenDriver = {
    .name   = "libvixen",
    .probe  = libvixen_probe,
    .attach = libvixen_attach,
    .detach = libvixen_detach,
};

int libvixen_usbcharge_probe(int device_id) { return SCE_USBD_PROBE_FAILED; }
int libvixen_usbcharge_attach(int device_id) { return SCE_USBD_PROBE_FAILED; }
int libvixen_usbcharge_detach(int device_id) { return SCE_USBD_PROBE_FAILED; }

static const SceUsbdDriver libvixenFakeUsbchargeDriver = {
    .name   = "usb_charge",
    .probe  = libvixen_usbcharge_probe,
    .attach = libvixen_usbcharge_attach,
    .detach = libvixen_usbcharge_detach,
};

int libvixen_probe(int device_id)
{
  SceUsbdDeviceDescriptor *device;
  ksceDebugPrintf("probing device: %x\n", device_id);
  device = (SceUsbdDeviceDescriptor *)ksceUsbdScanStaticDescriptor(device_id, 0, SCE_USBD_DESCRIPTOR_DEVICE);
  if (device)
  {
    ksceDebugPrintf("vendor: %04x\n", device->idVendor);
    ksceDebugPrintf("product: %04x\n", device->idProduct);
    int i;
    for (i = 0; _devices[i].type != PAD_UNKNOWN; i++)
    {
      if (_devices[i].idVendor == device->idVendor && _devices[i].idProduct == device->idProduct)
        break;
    }

    if (_devices[i].type == PAD_UNKNOWN)
    {
      // TODO: try generic?
      ksceDebugPrintf("Not supported!\n");
      return SCE_USBD_PROBE_FAILED;
    }
    return SCE_USBD_PROBE_SUCCEEDED;
  }
  return SCE_USBD_PROBE_FAILED;
}

int libvixen_attach(int device_id)
{
  SceUsbdDeviceDescriptor *device;
  ksceDebugPrintf("attaching device: %x\n", device_id);
  device = (SceUsbdDeviceDescriptor *)ksceUsbdScanStaticDescriptor(device_id, 0, SCE_USBD_DESCRIPTOR_DEVICE);
  if (device)
  {
    ksceDebugPrintf("vendor: %04x\n", device->idVendor);
    ksceDebugPrintf("product: %04x\n", device->idProduct);
    int i;
    for (i = 0; _devices[i].type != PAD_UNKNOWN; i++)
    {
      if (_devices[i].idVendor == device->idVendor && _devices[i].idProduct == device->idProduct)
      {
        ksceDebugPrintf("Found, attaching\n");
        break;
      }
    }

    if (_devices[i].type == PAD_UNKNOWN)
    {
      // TODO: try generic?
      return SCE_USBD_ATTACH_FAILED;
    }

    // wireless takes all 4 ports, sorry
    if (_devices[i].type == PAD_XBOX360W)
    {

      for (int cont = 0; cont < MAX_CONTROLLERS; cont++)
      {
        Xbox360WController_probe(&controllers[cont], device_id, cont);
        if (!controllers[cont].inited)
        {
          ksceDebugPrintf("Can't init gamepad (wireless)\n");
          return SCE_USBD_ATTACH_FAILED;
        }
      }
      ksceDebugPrintf("Attached!\n");
      return SCE_USBD_ATTACH_SUCCEEDED;
    }
    else
    {
      int cont = -1;

      // find free slot
      for (int i = 0; i < MAX_CONTROLLERS; i++)
      {
        if (!controllers[i].attached || !controllers[i].inited)
        {
          cont = i;
          break;
        }
      }

      if (cont == -1)
      {
        return SCE_USBD_ATTACH_FAILED;
      }

      if (!controllers[cont].attached)
      {
        switch (_devices[i].type)
        {
          case PAD_XBOX:
            XboxController_probe(&controllers[cont], device_id, cont);
            break;
          case PAD_XBOX360:
            Xbox360Controller_probe(&controllers[cont], device_id, cont);
            break;
          case PAD_DS3:
            DS3Controller_probe(&controllers[cont], device_id, cont);
            break;
          case PAD_DINPUT:
            DinputController_probe(&controllers[cont], device_id, cont, device->idVendor, device->idProduct);
            break;
          default:
            break;
        }

        // something gone wrong during usb init
        if (!controllers[cont].inited)
        {
          ksceDebugPrintf("Can't init gamepad (wired)\n");
          return SCE_USBD_ATTACH_FAILED;
        }
        ksceDebugPrintf("Attached!\n");
        controllers[cont].processReport = _devices[i].processReport;

        /*
         * Assign/reassign PSTV ports immediately after the USB controller
         * becomes active. The controller-port-info hook will keep this map
         * synchronized when native Sony controllers appear or disappear.
         */
        rebuild_vixen_port_map();
        return SCE_USBD_ATTACH_SUCCEEDED;
      }
    }
  }
  return SCE_USBD_ATTACH_FAILED;
}

int libvixen_detach(int device_id)
{

  for (int i = 0; i < MAX_CONTROLLERS; i++)
  {
    if (controllers[i].inited && controllers[i].device_id == device_id)
    {
      if (controllers[i].type == PAD_XBOX360W)
      {
        for (int j = 0; j < MAX_CONTROLLERS; j++)
        {
          if (controllers[j].inited)
          {
            controllers[j].attached     = 0;
            controllers[j].inited       = 0;
            controllers[i].pipe_in      = 0;
            controllers[i].pipe_out     = 0;
            controllers[i].pipe_control = 0;
          }
        }
      }
      else
      {
        controllers[i].attached     = 0;
        controllers[i].inited       = 0;
        controllers[i].pipe_in      = 0;
        controllers[i].pipe_out     = 0;
        controllers[i].pipe_control = 0;
      }
      return SCE_USBD_DETACH_SUCCEEDED;
    }
  }
  return SCE_USBD_DETACH_FAILED;
}

static int libvixen_sysevent_handler(int resume, int eventid, void *args, void *opt)
{
  if (resume && started)
  {
    if (ksceSblAimgrIsGenuineVITA())
      ksceUsbServMacSelect(2, 0); // re-set host mode
  }
  else if (started && eventid == 256)
  {
    // turn off wireless controllers
    for (int i = 0; i < MAX_CONTROLLERS; i++)
    {
      if (controllers[i].attached && controllers[i].type == PAD_XBOX360W)
      {
        Xbox360WController_turnOff(&controllers[i]);
      }
    }
  }
  return 0;
}

int module_start(SceSize args, void *argp)
{
  tai_module_info_t modInfo;
  modInfo.size = sizeof(tai_module_info_t);

  for (int i = 0; i < MAX_CONTROLLERS; i++)
  {
    controllers[i].inited   = 0;
    controllers[i].attached = 0;
    vixen_vita_port[i]      = 0;
  }

  // Hook controller info functions
  BIND_FUNC_EXPORT_HOOK(ksceCtrlGetControllerPortInfo, KERNEL_PID, "SceCtrl", TAI_ANY_LIBRARY, 0xF11D0D30);
  BIND_FUNC_EXPORT_HOOK(sceCtrlGetBatteryInfo, KERNEL_PID, "SceCtrl", TAI_ANY_LIBRARY, 0x8F9B1CE5);
  BIND_FUNC_EXPORT_HOOK(sceCtrlSetActuator, KERNEL_PID, "SceCtrl", TAI_ANY_LIBRARY, 0xDBCAA0C9);

  if (taiGetModuleInfoForKernel(KERNEL_PID, "SceCtrl", &modInfo) < 0)
    return SCE_KERNEL_START_FAILED;

  // Hook control data functions
  BIND_FUNC_EXPORT_HOOK(ksceCtrlPeekBufferPositive, KERNEL_PID, "SceCtrl", TAI_ANY_LIBRARY, 0xEA1D3A34);
  BIND_FUNC_EXPORT_HOOK(ksceCtrlReadBufferPositive, KERNEL_PID, "SceCtrl", TAI_ANY_LIBRARY, 0x9B96A1AA);
  BIND_FUNC_EXPORT_HOOK(ksceCtrlPeekBufferNegative, KERNEL_PID, "SceCtrl", TAI_ANY_LIBRARY, 0x19895843);
  BIND_FUNC_EXPORT_HOOK(ksceCtrlReadBufferNegative, KERNEL_PID, "SceCtrl", TAI_ANY_LIBRARY, 0x8D4E0DD1);
  BIND_FUNC_OFFSET_HOOK(ksceCtrlPeekBufferPositiveExt, KERNEL_PID, modInfo.modid, 0, 0x3928 | 1, 1);
  BIND_FUNC_OFFSET_HOOK(ksceCtrlReadBufferPositiveExt, KERNEL_PID, modInfo.modid, 0, 0x3BCC | 1, 1);

  // Hook extended control data functions
  BIND_FUNC_OFFSET_HOOK(ksceCtrlPeekBufferPositive2, KERNEL_PID, modInfo.modid, 0, 0x3EF8 | 1, 1);
  BIND_FUNC_OFFSET_HOOK(ksceCtrlReadBufferPositive2, KERNEL_PID, modInfo.modid, 0, 0x449C | 1, 1);
  BIND_FUNC_OFFSET_HOOK(ksceCtrlPeekBufferNegative2, KERNEL_PID, modInfo.modid, 0, 0x41C8 | 1, 1);
  BIND_FUNC_OFFSET_HOOK(ksceCtrlReadBufferNegative2, KERNEL_PID, modInfo.modid, 0, 0x47F0 | 1, 1);
  BIND_FUNC_OFFSET_HOOK(ksceCtrlPeekBufferPositiveExt2, KERNEL_PID, modInfo.modid, 0, 0x4B48 | 1, 1);
  BIND_FUNC_OFFSET_HOOK(ksceCtrlReadBufferPositiveExt2, KERNEL_PID, modInfo.modid, 0, 0x4E14 | 1, 1);

  BIND_FUNC_EXPORT_HOOK(sceCtrlDisconnect, KERNEL_PID, "SceCtrl", TAI_ANY_LIBRARY, 0x16D26DC7);

  started = 1;

  if (ksceSblAimgrIsGenuineVITA())
  {
    ksceUsbServMacSelect(2, 0);
  }

  int ret_drv = ksceUsbdRegisterDriver(&libvixenDriver);
  ksceDebugPrintf("ksceUsbdRegisterDriver = 0x%08x\n", ret_drv);

  // remove sony usb_charge driver that intercepts HID devices
  ret_drv = ksceUsbdUnregisterDriver(&libvixenFakeUsbchargeDriver);
  ksceDebugPrintf("ksceUsbdUnregisterDriver = 0x%08x\n", ret_drv);

  ksceKernelRegisterSysEventHandler("zvixen_sysevent", libvixen_sysevent_handler, NULL);

  return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize args, void *argp)
{
  // Unhook controller info functions
  UNBIND_FUNC_HOOK(ksceCtrlGetControllerPortInfo);
  UNBIND_FUNC_HOOK(sceCtrlGetBatteryInfo);
  UNBIND_FUNC_HOOK(sceCtrlSetActuator);
  UNBIND_FUNC_HOOK(sceCtrlDisconnect);

  // Unhook control data functions
  UNBIND_FUNC_HOOK(ksceCtrlReadBufferNegative);
  UNBIND_FUNC_HOOK(ksceCtrlPeekBufferPositive);
  UNBIND_FUNC_HOOK(ksceCtrlReadBufferPositive);
  UNBIND_FUNC_HOOK(ksceCtrlPeekBufferNegative);
  UNBIND_FUNC_HOOK(ksceCtrlPeekBufferPositiveExt);
  UNBIND_FUNC_HOOK(ksceCtrlReadBufferPositiveExt);

  // Unhook extended control data functions
  UNBIND_FUNC_HOOK(ksceCtrlPeekBufferPositive2);
  UNBIND_FUNC_HOOK(ksceCtrlReadBufferPositive2);
  UNBIND_FUNC_HOOK(ksceCtrlPeekBufferNegative2);
  UNBIND_FUNC_HOOK(ksceCtrlReadBufferNegative2);
  UNBIND_FUNC_HOOK(ksceCtrlPeekBufferPositiveExt2);
  UNBIND_FUNC_HOOK(ksceCtrlReadBufferPositiveExt2);

  return SCE_KERNEL_STOP_SUCCESS;
}

void _start()
{
  module_start(0, NULL);
}

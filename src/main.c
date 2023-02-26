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
#include <psp2kern/sblaimgr.h>
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
    // Spoof connected controllers to be DualShock 3 controllers
    for (int i = 0; i < MAX_CONTROLLERS; i++)
    {
      if (controllers[i].inited && controllers[i].attached)
        info->port[i + 1] = SCE_CTRL_TYPE_DS3; // no touch, so ds3
    }
  }

  return ret;
}

DECL_FUNC_HOOK(sceCtrlGetBatteryInfo, int port, uint8_t *batt)
{
  if (port > 0 && controllers[port - 1].attached && controllers[port - 1].inited)
  {
    // Override the battery level for connected controllers
    uint8_t data;
    ksceKernelMemcpyUserToKernel(&data, (void *)batt, sizeof(uint8_t));
    data = controllers[port - 1].battery_level;
    ksceKernelMemcpyKernelToUser((void *)batt, &data, sizeof(uint8_t));
    return 0;
  }

  return TAI_CONTINUE(int, sceCtrlGetBatteryInfoHookRef, port, batt);
}

DECL_FUNC_HOOK(sceCtrlSetActuator, int port, const SceCtrlActuator *pState)
{
  if (port > 0 && controllers[port - 1].attached && controllers[port - 1].inited)
  {
    SceCtrlActuator lpState;
    ksceKernelMemcpyUserToKernel(&lpState, (void *)pState, sizeof(SceCtrlActuator));
    switch (controllers[port - 1].type)
    {
      case PAD_XBOX360:
        Xbox360Controller_setRumble(&controllers[port - 1], lpState.small, lpState.large);
        break;
      case PAD_DS3:
        DS3Controller_setRumble(&controllers[port - 1], lpState.small, lpState.large);
        break;
      case PAD_XBOX360W:
        Xbox360WController_setRumble(&controllers[port - 1], lpState.small, lpState.large);
        break;
      case PAD_XBOX:
        XboxController_setRumble(&controllers[port - 1], lpState.small, lpState.large);
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
  if (port > 0 && controllers[port - 1].attached && controllers[port - 1].inited)
  {
    if (controllers[port - 1].type == PAD_XBOX360W)
    {
      Xbox360WController_turnOff(&controllers[port - 1]);
      return 0;
    }
  }
  return TAI_CONTINUE(int, sceCtrlDisconnectHookRef, port);
}

static void patchControlData(int port, SceCtrlData *data, int count, uint8_t negative, uint8_t triggers_ext)
{
  // Use controller 1 data for port 0, or controllers 1-4 for ports 1-4
  int cont = (port > 0) ? (port - 1) : 0;

  if (!controllers[cont].inited || !controllers[cont].attached)
    return;

  const ControlData *controlData = &(controllers[cont].controlData);

  // Forward PS button presses to the kernel so the system menu receives them
  if (controlData->buttons & SCE_CTRL_PSBUTTON)
    ksceCtrlSetButtonEmulation(port, 0, 0, SCE_CTRL_PSBUTTON, 16);

  for (int i = 0; i < count; i++)
  {
    // Reset initial values for controller ports (port 0 is additive)
    if (port > 0)
    {
      data[i].buttons = (negative ? 0xFFFFFFFF : 0x00000000);
      data[i].lx = data[i].ly = data[i].rx = data[i].ry = 127;
    }

    // Set the button data from the controller, with optional negative logic
    if (negative)
      data[i].buttons &= ~controlData->buttons;
    else
      data[i].buttons |= controlData->buttons;

    data[i].lt = clamp(controlData->lt, 0, 255);
    data[i].rt = clamp(controlData->rt, 0, 255);

    // Set the stick data from the controller
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
        return SCE_USBD_ATTACH_FAILED;

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

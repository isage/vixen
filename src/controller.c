#include "controller.h"
#include "devicelist.h"

#include "controllers/dinput_controller.h"
#include "controllers/ds3_controller.h"
#include "controllers/xbox_360_controller.h"
#include "controllers/xbox_360w_controller.h"
#include "controllers/xbox_controller.h"

#include <psp2kern/kernel/suspend.h>
#include <psp2kern/kernel/threadmgr.h>
#include <psp2kern/usbd.h>

void on_read_data(int32_t result, int32_t count, void *arg)
{
  Controller *c = (Controller *)arg;

  if (!c)
    return;

  if (result == 0 && count > 0 && c->inited)
  {
    int ret = 0;
    switch (c->type)
    {
      case PAD_XBOX:
        ret = XboxController_processReport(c, count);
        break;
      case PAD_XBOX360:
        ret = Xbox360Controller_processReport(c, count);
        break;
      case PAD_XBOX360W:
        ret = Xbox360WController_processReport(c, count);
        break;
      case PAD_DS3:
        ret = DS3Controller_processReport(c, count);
        break;
      case PAD_DINPUT:
        ret = DinputController_processReport(c, count);
        break;
      default:
        break;
    }

    if (ret)
      ksceKernelPowerTick(0);
  }

  /*
   * A failed asynchronous transfer may indicate that the USB device has
   * disconnected or re-enumerated without a detach callback. Do not keep
   * resubmitting transfers against the stale endpoint. Retire the slot so
   * that a subsequently attached device can reuse it.
   *
   * Successful zero-length completions are resubmitted normally.
   */
  if (result != 0)
  {
    c->attached = 0;
    c->inited   = 0;

    c->controlData.buttons = 0;
    c->controlData.leftX   = 128;
    c->controlData.leftY   = 128;
    c->controlData.rightX  = 128;
    c->controlData.rightY  = 128;
    c->controlData.lt      = 0;
    c->controlData.rt      = 0;

    return;
  }

  usb_read(c);
}

void on_write_data(int32_t result, int32_t count, void *arg)
{
  // check status
  // do nothing?
}

void usb_read(Controller *c)
{
  int ret;
  if (!c)
    return;
  if (!c->inited)
    return;

  ret = ksceUsbdInterruptTransfer(c->pipe_in, c->buffer, c->buffer_size, on_read_data, c);
  if (ret < 0)
  {
    ksceDebugPrintf("ksceUsbdInterruptTransfer(in) error: 0x%08x\n", ret);
  }
}

void usb_write(Controller *c, uint8_t *data, int len)
{
  int ret;
  ret = ksceUsbdInterruptTransfer(c->pipe_out, data, len, on_write_data, c);

  if (ret < 0)
  {
    ksceDebugPrintf("ksceUsbdInterruptTransfer(out) error: 0x%08x\n", ret);
    // error out
  }
}

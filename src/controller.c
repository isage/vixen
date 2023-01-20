#include "controller.h"

#include "controllers/ds3_controller.h"
#include "controllers/xbox_360_controller.h"
#include "controllers/xbox_360w_controller.h"

#include <psp2kern/kernel/suspend.h>
#include <psp2kern/kernel/threadmgr.h>
#include <psp2kern/usbd.h>

void on_read_data(int32_t result, int32_t count, void *arg)
{
  // process buffer
  Controller *c = (Controller *)arg;
  if (result == 0 && count > 0 && arg)
  {
    if (c->inited)
    {
      if (c->type == PAD_XBOX360)
      {
        if (Xbox360Controller_processReport(c, count))
          ksceKernelPowerTick(0); // cancel sleep timers.
      }
      else if (c->type == PAD_DS3)
      {
        if (DS3Controller_processReport(c, count))
          ksceKernelPowerTick(0); // cancel sleep timers.
      }
      else
      {
        if (Xbox360WController_processReport(c, count))
          ksceKernelPowerTick(0); // cancel sleep timers.
      }
    }
  }

  usb_read(c);
}

void on_write_data(int32_t result, int32_t count, void *arg)
{
  //    ksceDebugPrintf("write status: %d %d\n", result, count);
  // check status
  // do nothing?
}

void usb_read(Controller *c)
{
  int ret;

  if (!c->inited)
    return;

  ret = ksceUsbdInterruptTransfer(c->pipe_in, c->buffer, c->buffer_size, on_read_data, c);

  if (ret < 0)
  {
    ksceDebugPrintf("ksceUsbdInterruptTransfer(in) error: 0x%08x\n", ret);
    // error out
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

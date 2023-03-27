#include "xbox_360w_controller.h"

#include "utils/bit.h"
#include "xbox_360_report.h"

#include <psp2kern/ctrl.h>
#include <psp2kern/kernel/debug.h>

#define USB_ENDPOINT_OUT 0x01
#define USB_ENDPOINT_IN 0x81

uint8_t Xbox360WController_probe(Controller *c, int device_id, int port)
{
  c->type          = PAD_XBOX360W;
  c->buffer_size   = 32;
  c->device_id     = device_id;
  c->port          = port;
  c->battery_level = 5;

  // init endpoints and stuff
  SceUsbdEndpointDescriptor *endpoint;
#if defined(DEBUG)
  ksceDebugPrintf("scanning endpoints\n");
#endif
  endpoint = (SceUsbdEndpointDescriptor *)ksceUsbdScanStaticDescriptor(device_id, 0, SCE_USBD_DESCRIPTOR_ENDPOINT);
  while (endpoint)
  {
#if defined(DEBUG)
    ksceDebugPrintf("got EP: %02x\n", endpoint->bEndpointAddress);
#endif
    if (endpoint->bEndpointAddress == USB_ENDPOINT_IN + port)
    {
#if defined(DEBUG)
      ksceDebugPrintf("opening in pipe\n");
#endif
      c->pipe_in = ksceUsbdOpenPipe(device_id, endpoint);
#if defined(DEBUG)
      ksceDebugPrintf("= 0x%08x\n", c->pipe_in);
      ksceDebugPrintf("bmAttributes = %x\n", endpoint->bmAttributes);
#endif
    }
    else if (endpoint->bEndpointAddress == USB_ENDPOINT_OUT + port)
    {
#if defined(DEBUG)
      ksceDebugPrintf("opening out pipe\n");
#endif
      c->pipe_out = ksceUsbdOpenPipe(device_id, endpoint);
#if defined(DEBUG)
      ksceDebugPrintf("= 0x%08x\n", c->pipe_out);
      ksceDebugPrintf("bmAttributes = %x\n", endpoint->bmAttributes);
#endif
    }

    if (c->pipe_in > 0 && c->pipe_out > 0)
      break;

    endpoint
        = (SceUsbdEndpointDescriptor *)ksceUsbdScanStaticDescriptor(device_id, endpoint, SCE_USBD_DESCRIPTOR_ENDPOINT);
  }

  if (c->pipe_in > 0 && c->pipe_out > 0)
  {
    SceUsbdConfigurationDescriptor *cdesc;
    if ((cdesc = (SceUsbdConfigurationDescriptor *)ksceUsbdScanStaticDescriptor(device_id, NULL,
                                                                                SCE_USBD_DESCRIPTOR_CONFIGURATION))
        == NULL)
      return 0;

    SceUID control_pipe_id = ksceUsbdOpenPipe(device_id, NULL);
    // set default config
    int r = ksceUsbdSetConfiguration(control_pipe_id, cdesc->bConfigurationValue, NULL, NULL);
#if defined(DEBUG)
    ksceDebugPrintf("ksceUsbdSetConfiguration = 0x%08x\n", r);
#endif
    if (r < 0)
      return 0;
    c->attached = 0;
    c->inited   = 1;
  }

  usb_read(c);
  return 1;
}

void Xbox360WController_setLed(Controller *c, uint8_t led)
{
  uint8_t ledcmd[] __attribute__((aligned(64)))
  = {0x00, 0x00, 0x08, (uint8_t)(0x40 + led), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  usb_write(c, ledcmd, 12);
}

void Xbox360WController_setRumble(Controller *c, uint8_t small, uint8_t large)
{
  uint8_t rumblecmd[] __attribute__((aligned(64)))
  = {0x00, 0x01, 0x0f, 0xc0, 0x00, small, large, 0x00, 0x00, 0x00, 0x00, 0x00};

  usb_write(c, rumblecmd, 12);
}

void Xbox360WController_turnOff(Controller *c)
{
  uint8_t powercmd[] __attribute__((aligned(64)))
  = {0x00, 0x00, 0x08, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  usb_write(c, powercmd, 12);
  c->attached = 0;
}

uint8_t Xbox360WController_processReport(Controller *c, size_t length)
{
  if (length == 2 && c->buffer[0] == 0x08)
  { // Connection Status Message
    if (c->buffer[1] == 0x00)
    {
      ksceDebugPrintf("controler disconnected\n");

      // reset the controller into neutral position on disconnect
      c->controlData.buttons = 0;
      c->controlData.leftX   = 127;
      c->controlData.leftY   = 127;
      c->controlData.rightX  = 127;
      c->controlData.rightY  = 127;
      c->controlData.lt      = 0;
      c->controlData.rt      = 0;
      c->attached            = 0;

      return 1;
    }
    else if (c->buffer[1] == 0x80)
    {
      ksceDebugPrintf("connection status: controller connected\n");
      Xbox360WController_setLed(c, c->port + 2);
      c->attached = 1;
      return 1;
    }
    else if (c->buffer[1] == 0x40)
    {
      ksceDebugPrintf("Connection status: headset connected\n");
      return 1;
    }
    else if (c->buffer[1] == 0xc0)
    {
      ksceDebugPrintf("Connection status: controller and headset connected\n");
      Xbox360WController_setLed(c, c->port + 2);
      c->attached = 1;
      return 1;
    }
    else
    {
      ksceDebugPrintf("Connection status: unknown\n");
      return 1;
    }
  }
  else if (length == 29)
  {
    if (c->buffer[0] == 0x00 && c->buffer[1] == 0x0f && c->buffer[2] == 0x00 && c->buffer[3] == 0xf0)
    {
      // Initial Announce Message
      c->battery_level = c->buffer[17] / 44; // crudely map 0-255 -> 0-5
    }
    else if (c->buffer[0] == 0x00 && c->buffer[1] == 0x00 && c->buffer[2] == 0x00 && c->buffer[3] == 0x13)
    {
      // Battery status
      c->battery_level = c->buffer[4] / 44;
    }
    else if (c->buffer[0] == 0x00 && c->buffer[1] == 0x01 && c->buffer[2] == 0x00 && c->buffer[3] == 0xf0
             && c->buffer[4] == 0x00 && c->buffer[5] == 0x13)
    {
      // Event message
      uint8_t *ptr = c->buffer + 4;
      Xbox360Report report;

      report.dpad_up    = bit(ptr + 2, 0);
      report.dpad_down  = bit(ptr + 2, 1);
      report.dpad_left  = bit(ptr + 2, 2);
      report.dpad_right = bit(ptr + 2, 3);

      report.start   = bit(ptr + 2, 4);
      report.back    = bit(ptr + 2, 5);
      report.thumb_l = bit(ptr + 2, 6);
      report.thumb_r = bit(ptr + 2, 7);

      report.lb    = bit(ptr + 3, 0);
      report.rb    = bit(ptr + 3, 1);
      report.guide = bit(ptr + 3, 2);

      report.a = bit(ptr + 3, 4);
      report.b = bit(ptr + 3, 5);
      report.x = bit(ptr + 3, 6);
      report.y = bit(ptr + 3, 7);

      report.lt = ptr[4];
      report.rt = ptr[5];

      report.x1 = *((int16_t *)(ptr + 6));
      report.y1 = *((int16_t *)(ptr + 8));

      report.x2 = *((int16_t *)(ptr + 10));
      report.y2 = *((int16_t *)(ptr + 12));

      c->controlData.buttons = 0;

      if (report.a)
        c->controlData.buttons |= SCE_CTRL_CROSS;
      if (report.b)
        c->controlData.buttons |= SCE_CTRL_CIRCLE;
      if (report.y)
        c->controlData.buttons |= SCE_CTRL_TRIANGLE;
      if (report.x)
        c->controlData.buttons |= SCE_CTRL_SQUARE;

      if (report.dpad_up)
        c->controlData.buttons |= SCE_CTRL_UP;
      if (report.dpad_down)
        c->controlData.buttons |= SCE_CTRL_DOWN;
      if (report.dpad_left)
        c->controlData.buttons |= SCE_CTRL_LEFT;
      if (report.dpad_right)
        c->controlData.buttons |= SCE_CTRL_RIGHT;

      if (report.lb)
        c->controlData.buttons |= SCE_CTRL_L1;
      if (report.rb)
        c->controlData.buttons |= SCE_CTRL_R1;
      if (report.thumb_l)
        c->controlData.buttons |= SCE_CTRL_L3;
      if (report.thumb_r)
        c->controlData.buttons |= SCE_CTRL_R3;

      if (report.lt > 0)
        c->controlData.buttons |= SCE_CTRL_LTRIGGER;
      if (report.rt > 0)
        c->controlData.buttons |= SCE_CTRL_RTRIGGER;

      if (report.start)
        c->controlData.buttons |= SCE_CTRL_START;
      if (report.back)
        c->controlData.buttons |= SCE_CTRL_SELECT;
      if (report.guide)
        c->controlData.buttons |= SCE_CTRL_PSBUTTON;

      c->controlData.leftX  = report.x1 / 256 + 128;
      c->controlData.leftY  = report.y1 / 256 + 128;
      c->controlData.rightX = report.x2 / 256 + 128;
      c->controlData.rightY = report.y2 / 256 + 128;

      // up and down are reversed
      c->controlData.leftY  = 255 - c->controlData.leftY;
      c->controlData.rightY = 255 - c->controlData.rightY;

      c->controlData.lt = report.lt;
      c->controlData.rt = report.rt;

      return 1;
    }
    else
    {
#if defined(DEBUG)
//        ksceDebugPrintf("unknown packet\n");
#endif
    }
  }
  else
  {
#if defined(DEBUG)
//      ksceDebugPrintf("unknown packet\n");
#endif
  }

  return 0;
}

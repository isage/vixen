#include "xbox_360_controller.h"
#include "../devicelist.h"

#include "utils/bit.h"
#include "xbox_360_report.h"

#include <psp2kern/ctrl.h>
#include <psp2kern/kernel/debug.h>

#define USB_IF_PROTOCOL 0x01

uint8_t Xbox360Controller_probe(Controller *c, int device_id, int port)
{
  c->type          = PAD_XBOX360;
  c->buffer_size   = 32;
  c->device_id     = device_id;
  c->port          = port;
  c->battery_level = 5;

#if defined(DEBUG)
  ksceDebugPrintf("scanning endpoints for device %d\n", device_id);
#endif

  // init endpoints and stuff

  SceUsbdInterfaceDescriptor *interface;
  interface = (SceUsbdInterfaceDescriptor *)ksceUsbdScanStaticDescriptor(device_id, 0, SCE_USBD_DESCRIPTOR_ENDPOINT);
  while (interface)
  {
    if (interface->bInterfaceProtocol == USB_IF_PROTOCOL)
      break;
    interface = (SceUsbdInterfaceDescriptor *)ksceUsbdScanStaticDescriptor(device_id, interface,
                                                                           SCE_USBD_DESCRIPTOR_ENDPOINT);
  }

  SceUsbdEndpointDescriptor *endpoint;

  endpoint
      = (SceUsbdEndpointDescriptor *)ksceUsbdScanStaticDescriptor(device_id, interface, SCE_USBD_DESCRIPTOR_ENDPOINT);
  while (endpoint)
  {
#if defined(DEBUG)
    ksceDebugPrintf("got EP: %02x\n", endpoint->bEndpointAddress);
#endif
    if ((endpoint->bEndpointAddress & SCE_USBD_ENDPOINT_DIRECTION_BITS) == SCE_USBD_ENDPOINT_DIRECTION_IN)
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
    else if ((endpoint->bEndpointAddress & SCE_USBD_ENDPOINT_DIRECTION_BITS) == SCE_USBD_ENDPOINT_DIRECTION_OUT)
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
    c->attached = 1;
    c->inited   = 1;
  }

  usb_read(c);
  return 1;
}

void Xbox360Controller_setRumble(Controller *c, uint8_t small, uint8_t large)
{
  uint8_t rumblecmd[] __attribute__((aligned(64))) = {0x00, 0x08, 0x00, small, large, 0x00, 0x00, 0x00};

  usb_write(c, rumblecmd, 8);
}

uint8_t Xbox360Controller_processReport(Controller *c, size_t length)
{
  if (length == 3 && c->buffer[0] == 0x01 && c->buffer[1] == 0x03)
  {
#if defined(DEBUG)
    ksceDebugPrintf("Xbox360Controller: LED Status: %d\n", (c->buffer[2]));
#endif
  }
  else if (length == 3 && c->buffer[0] == 0x03 && c->buffer[1] == 0x03)
  {
#if defined(DEBUG)
    ksceDebugPrintf("rumble status: %d\n", (c->buffer[2]));
#endif
  }
  else if (length == 3 && c->buffer[0] == 0x08 && c->buffer[1] == 0x03)
  {
#if defined(DEBUG)
    if (c->buffer[2] == 0x00)
    {
      ksceDebugPrintf("peripheral: none");
    }
    else if (c->buffer[2] == 0x01)
    {
      ksceDebugPrintf("peripheral: chatpad");
    }
    else if (c->buffer[2] == 0x02)
    {
      ksceDebugPrintf("peripheral: headset");
    }
    else if (c->buffer[2] == 0x03)
    {
      ksceDebugPrintf("peripheral: headset, chatpad");
    }
    else
    {
      ksceDebugPrintf("peripheral: unknown: %d", (c->buffer[2]));
    }
#endif
  }
  else if (length == 20 && c->buffer[0] == 0x00 && (c->buffer[1] == 0x14 || c->buffer[1] == 0x00))
  {
    Xbox360Report report;

    report.dpad_up    = bit(c->buffer + 2, 0);
    report.dpad_down  = bit(c->buffer + 2, 1);
    report.dpad_left  = bit(c->buffer + 2, 2);
    report.dpad_right = bit(c->buffer + 2, 3);

    report.start   = bit(c->buffer + 2, 4);
    report.back    = bit(c->buffer + 2, 5);
    report.thumb_l = bit(c->buffer + 2, 6);
    report.thumb_r = bit(c->buffer + 2, 7);

    report.lb    = bit(c->buffer + 3, 0);
    report.rb    = bit(c->buffer + 3, 1);
    report.guide = bit(c->buffer + 3, 2);

    report.a = bit(c->buffer + 3, 4);
    report.b = bit(c->buffer + 3, 5);
    report.x = bit(c->buffer + 3, 6);
    report.y = bit(c->buffer + 3, 7);

    report.lt = c->buffer[4];
    report.rt = c->buffer[5];

    report.x1 = *((int16_t *)(c->buffer + 6));
    report.y1 = *((int16_t *)(c->buffer + 8));

    report.x2 = *((int16_t *)(c->buffer + 10));
    report.y2 = *((int16_t *)(c->buffer + 12));

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
//        ksceDebugPrintf("unknown report. len = %d\n", length);
  }
  return 0;
}

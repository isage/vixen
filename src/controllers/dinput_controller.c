#include "dinput_controller.h"

#include "../unpack.h"
#include "xbox_360_report.h"

#include <psp2kern/ctrl.h>
#include <psp2kern/kernel/debug.h>

//#define USB_ENDPOINT_OUT 0x01
//#define USB_ENDPOINT_IN 0x81

uint8_t DinputController_probe(Controller *c, int device_id, int port, int vendor, int product)
{
  c->type          = PAD_DINPUT;
  c->buffer_size   = 8;
  c->device_id     = device_id;
  c->port          = port;
  c->battery_level = 5;
  c->vendor        = vendor;
  c->product       = product;

  // init endpoints and stuff
  SceUsbdEndpointDescriptor *endpoint;
#if defined(DEBUG)
  ksceDebugPrintf("scanning endpoints for device %d\n", device_id);
#endif
  endpoint = (SceUsbdEndpointDescriptor *)ksceUsbdScanStaticDescriptor(device_id, 0, SCE_USBD_DESCRIPTOR_ENDPOINT);
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
      c->buffer_size = endpoint->wMaxPacketSize;
      if (c->buffer_size > 64)
      {
        ksceDebugPrintf("Packet size too big = %d\n", endpoint->wMaxPacketSize);
        return 0;
      }
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

  if (c->pipe_in > 0)
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

static void process_dpad_angle(Controller *c, int idx)
{
    uint8_t dpad = c->buffer[idx] & 0x0F;
    switch (dpad)
    {
      case 0x0:
        c->controlData.buttons |= SCE_CTRL_UP;
        break;
      case 0x1:
        c->controlData.buttons |= SCE_CTRL_UP;
        c->controlData.buttons |= SCE_CTRL_RIGHT;
        break;
      case 0x2:
        c->controlData.buttons |= SCE_CTRL_RIGHT;
        break;
      case 0x3:
        c->controlData.buttons |= SCE_CTRL_RIGHT;
        c->controlData.buttons |= SCE_CTRL_DOWN;
        break;
      case 0x4:
        c->controlData.buttons |= SCE_CTRL_DOWN;
        break;
      case 0x5:
        c->controlData.buttons |= SCE_CTRL_DOWN;
        c->controlData.buttons |= SCE_CTRL_LEFT;
        break;
      case 0x6:
        c->controlData.buttons |= SCE_CTRL_LEFT;
        break;
      case 0x7:
        c->controlData.buttons |= SCE_CTRL_LEFT;
        c->controlData.buttons |= SCE_CTRL_UP;
        break;
      case 0x8:
      default:
        break;
    }

}

uint8_t DinputController_processReport(Controller *c, size_t length)
{
  // reset everything
  c->controlData.buttons = 0;
  c->controlData.leftX   = 128;
  c->controlData.leftY   = 128;
  c->controlData.rightX  = 128;
  c->controlData.rightY  = 128;
  c->controlData.lt      = 0;
  c->controlData.rt      = 0;

  if (c->vendor == 0x054c && c->product == 0x0cda) // playstation classic
  {
    uint8_t dpadx = (c->buffer[1] >> 2) & 0x03;
    uint8_t dpady = (c->buffer[1] >> 4) & 0x03;

    switch (dpadx)
    {
      case 0x0:
        c->controlData.buttons |= SCE_CTRL_LEFT;
        break;
      case 0x2:
        c->controlData.buttons |= SCE_CTRL_RIGHT;
        break;
      case 0x1:
      default:
        break;
    }

    switch (dpady)
    {
      case 0x0:
        c->controlData.buttons |= SCE_CTRL_UP;
        break;
      case 0x2:
        c->controlData.buttons |= SCE_CTRL_DOWN;
        break;
      case 0x1:
      default:
        break;
    }

    if (bit(c->buffer, 0))
      c->controlData.buttons |= SCE_CTRL_TRIANGLE;
    if (bit(c->buffer, 1))
      c->controlData.buttons |= SCE_CTRL_CIRCLE;
    if (bit(c->buffer, 2))
      c->controlData.buttons |= SCE_CTRL_CROSS;
    if (bit(c->buffer, 3))
      c->controlData.buttons |= SCE_CTRL_SQUARE;

    if (bit(c->buffer, 6))
      c->controlData.buttons |= SCE_CTRL_L1;
    if (bit(c->buffer, 7))
      c->controlData.buttons |= SCE_CTRL_R1;

    if (bit(c->buffer, 4))
    {
      c->controlData.buttons |= SCE_CTRL_LTRIGGER;
      c->controlData.lt = 0xFF;
    }

    if (bit(c->buffer, 5))
    {
      c->controlData.buttons |= SCE_CTRL_RTRIGGER;
      c->controlData.rt = 0xFF;
    }

    if (bit(c->buffer + 1, 1))
      c->controlData.buttons |= SCE_CTRL_START;
    if (bit(c->buffer + 1, 0))
      c->controlData.buttons |= SCE_CTRL_SELECT;

    if (bit(c->buffer, 6) && bit(c->buffer, 7) && bit(c->buffer + 1, 1)) // L+R+START combo
    {
      c->controlData.buttons |= SCE_CTRL_PSBUTTON;
      c->controlData.buttons &= ~SCE_CTRL_START;
    }

    return 1;
  }
  else if (c->vendor == 0x0F0D && c->product == 0x0049) // Hori ps3 mini diva
  {
    process_dpad_angle(c, 2);

    if (bit(c->buffer, 3))
      c->controlData.buttons |= SCE_CTRL_TRIANGLE;
    if (bit(c->buffer, 2))
      c->controlData.buttons |= SCE_CTRL_CIRCLE;
    if (bit(c->buffer, 1))
      c->controlData.buttons |= SCE_CTRL_CROSS;
    if (bit(c->buffer, 0))
      c->controlData.buttons |= SCE_CTRL_SQUARE;

    if (bit(c->buffer, 4))
      c->controlData.buttons |= SCE_CTRL_L1;
    if (bit(c->buffer, 5))
      c->controlData.buttons |= SCE_CTRL_R1;

    if (bit(c->buffer + 1, 2))
      c->controlData.buttons |= SCE_CTRL_L3;
    if (bit(c->buffer + 1, 3))
      c->controlData.buttons |= SCE_CTRL_R3;

    if (bit(c->buffer, 6))
    {
      c->controlData.buttons |= SCE_CTRL_LTRIGGER;
      c->controlData.lt = 0xFF;
    }

    if (bit(c->buffer, 7))
    {
      c->controlData.buttons |= SCE_CTRL_RTRIGGER;
      c->controlData.rt = 0xFF;
    }

    if (bit(c->buffer + 1, 1))
      c->controlData.buttons |= SCE_CTRL_START;
    if (bit(c->buffer + 1, 0))
      c->controlData.buttons |= SCE_CTRL_SELECT;

    if (bit(c->buffer + 1, 4))
      c->controlData.buttons |= SCE_CTRL_PSBUTTON;

    c->controlData.leftX  = c->buffer[3];
    c->controlData.leftY  = c->buffer[4];

    c->controlData.rightX = c->buffer[5];
    c->controlData.rightY = c->buffer[6];

    return 1;
  }
  else if (c->vendor == 0x046d && (c->product == 0xc216 || c->product == 0xc218)) // logitech
  {
    process_dpad_angle(c, 4);

    if (bit(c->buffer + 4, 5))
      c->controlData.buttons |= SCE_CTRL_CROSS;
    if (bit(c->buffer + 4, 6))
      c->controlData.buttons |= SCE_CTRL_CIRCLE;
    if (bit(c->buffer + 4, 7))
      c->controlData.buttons |= SCE_CTRL_TRIANGLE;
    if (bit(c->buffer + 4, 4))
      c->controlData.buttons |= SCE_CTRL_SQUARE;

    if (bit(c->buffer + 5, 0))
      c->controlData.buttons |= SCE_CTRL_L1;
    if (bit(c->buffer + 5, 1))
      c->controlData.buttons |= SCE_CTRL_R1;
    if (bit(c->buffer + 5, 6))
      c->controlData.buttons |= SCE_CTRL_L3;
    if (bit(c->buffer + 5, 7))
      c->controlData.buttons |= SCE_CTRL_R3;

    if (bit(c->buffer + 5, 2))
    {
      c->controlData.buttons |= SCE_CTRL_LTRIGGER;
      c->controlData.lt = 0xFF;
    }

    if (bit(c->buffer + 5, 3))
    {
      c->controlData.buttons |= SCE_CTRL_RTRIGGER;
      c->controlData.rt = 0xFF;
    }

    if (bit(c->buffer + 5, 5))
      c->controlData.buttons |= SCE_CTRL_START;
    if (bit(c->buffer + 5, 4))
      c->controlData.buttons |= SCE_CTRL_SELECT;

    if (bit(c->buffer + 5, 0) && bit(c->buffer + 5, 1) && bit(c->buffer + 5, 5)) // L+R+START combo
    {
      c->controlData.buttons |= SCE_CTRL_PSBUTTON;
      c->controlData.buttons &= ~SCE_CTRL_START;
    }

    c->controlData.leftX  = c->buffer[0];
    c->controlData.leftY  = c->buffer[1];
    c->controlData.rightX = c->buffer[2];
    c->controlData.rightY = c->buffer[3];
    return 1;
  }
  else
  {
    return 0;
  }
}

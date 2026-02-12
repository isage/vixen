#include "dinput_controller.h"
#include "../devicelist.h"

#include "utils/bit.h"
#include "xbox_360_report.h"

#include <psp2kern/ctrl.h>
#include <psp2kern/kernel/debug.h>

#define ksceUsbdGetHidDescriptor(pid, ptr, len, cb, arg) ({ \
        SceUsbdDeviceRequest _dr; \
        _dr.bmRequestType = 0x81; \
        _dr.bRequest = SCE_USBD_REQUEST_GET_DESCRIPTOR; \
        _dr.wValue = (0x22 << 8); \
        _dr.wIndex = 0; \
        _dr.wLength = (len); \
        ksceUsbdControlTransfer((pid), (&_dr), (ptr), (cb), (arg)); })

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

    // stupid 8bitdo requires reading hid descriptor
    if (c->vendor == 0x2dc8 && c->product == 0x3105)
        ksceUsbdGetHidDescriptor(control_pipe_id, NULL, 0, NULL, NULL);

    c->attached = 1;
    c->inited   = 1;
  }

  usb_read(c);
  return 1;
}

uint8_t DinputController_processReport(Controller *c, size_t length)
{
  // For Dual PSX Adaptor consider only first controller port
  if (c->vendor == 0x0810 && c->product == 0x0001 && c->buffer[0] != 1)
  {
    return 1;
  }

  // reset everything
  c->controlData.buttons = 0;
  c->controlData.leftX   = 128;
  c->controlData.leftY   = 128;
  c->controlData.rightX  = 128;
  c->controlData.rightY  = 128;
  c->controlData.lt      = 0;
  c->controlData.rt      = 0;

  if (c->processReport)
  {
    return c->processReport(c, length);
  }
  else
  {
    return 0;
  }
}

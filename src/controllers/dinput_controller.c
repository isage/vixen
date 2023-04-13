#include "dinput_controller.h"

#include "utils/bit.h"
#include "xbox_360_report.h"

#include <psp2kern/ctrl.h>
#include <psp2kern/kernel/debug.h>

#include "dinput/logitech.h"
#include "dinput/psclassic.h"
#include "dinput/horidiva.h"
#include "dinput/raphnetpsx.h"
#include "dinput/p3converter.h"

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
    return psclassic_processReport(c, length);;
  }
  else if (c->vendor == 0x0F0D && c->product == 0x0049) // Hori ps3 mini diva
  {
    return horiDiva_processReport(c, length);;
  }
  else if (c->vendor == 0x046d && (c->product == 0xc216 || c->product == 0xc218)) // logitech
  {
    return logitech_processReport(c, length);
  }
  else if (c->vendor == 0x289b && c->product == 0x0044) // raphnet
  {
    return raphnetpsx_processReport(c, length);
  }
  else if (c->vendor == 0x0e8f && c->product == 0x0003) // PIII Converter Model: 538
  {
    return p3converter_processReport(c, length);
  }
  else
  {
    return 0;
  }
}

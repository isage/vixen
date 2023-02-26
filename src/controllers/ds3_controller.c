#include "ds3_controller.h"

#include "../unpack.h"
#include "ds3_report.h"

#include <psp2kern/ctrl.h>
#include <psp2kern/kernel/debug.h>
#include <psp2kern/kernel/threadmgr.h>

#define HID_GET_REPORT 0x01
#define HID_GET_IDLE 0x02
#define HID_GET_PROTOCOL 0x03
#define HID_SET_REPORT 0x09
#define HID_SET_IDLE 0x0A
#define HID_SET_PROTOCOL 0x0B

#define HID_REPORT_TYPE_INPUT 0x01
#define HID_REPORT_TYPE_OUTPUT 0x02
#define HID_REPORT_TYPE_FEATURE 0x03

void oncontrol(int32_t result, int32_t count, void *arg)
{
#if defined(DEBUG)
  ksceDebugPrintf("oncontrol: %x %d\n", result, count);
#endif
}

uint8_t DS3Controller_probe(Controller *c, int device_id, int port)
{
  c->type          = PAD_DS3;
  c->buffer_size   = 64;
  c->device_id     = device_id;
  c->port          = port;
  c->battery_level = 5;

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
    c->pipe_control        = control_pipe_id;
    // set default config
    int r = ksceUsbdSetConfiguration(control_pipe_id, cdesc->bConfigurationValue, &oncontrol, NULL);
#if defined(DEBUG)
    ksceDebugPrintf("ksceUsbdSetConfiguration = 0x%08x\n", r);
#endif
    if (r < 0)
      return 0;
    c->attached = 1;
    c->inited   = 1;
  }

  // init command
  // TODO: use eventflag?
  ksceKernelDelayThread(500000);
  uint8_t cmd[] __attribute__((aligned(64)))
  = {0x42, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  SceUsbdDeviceRequest _dr;
  _dr.bmRequestType = 0x21;
  _dr.bRequest      = HID_SET_REPORT;
  _dr.wValue        = 0x03F4;
  _dr.wIndex        = 0;
  _dr.wLength       = 4;
  ksceUsbdControlTransfer(c->pipe_control, (&_dr), (uint8_t *)&cmd, &oncontrol, NULL);

  // TODO: use eventflag?
  ksceKernelDelayThread(500000);
  DS3Controller_setLed(c, port + 1);

  usb_read(c);
  return 1;
}

void DS3Controller_setRumble(Controller *c, uint8_t small, uint8_t large)
{
  uint8_t cmd[] __attribute__((aligned(64))) = {0x00, 0xFE,  small > 0 ? 0x01 : 0x00,
                                                0xFE, large, // rumble values
                                                0x00, 0x00,  0x00,
                                                0x00, 0x03, // 0x10=LED1 .. 0x02=LED4
                                                0xff, 0x27,  0x10,
                                                0x00, 0x32, // LED 4
                                                0xff, 0x27,  0x10,
                                                0x00, 0x32, // LED 3
                                                0xff, 0x27,  0x10,
                                                0x00, 0x32, // LED 2
                                                0xff, 0x27,  0x10,
                                                0x00, 0x32, // LED 1
                                                0x00, 0x00,  0x00,
                                                0x00, 0x00,  0x00,
                                                0x00, 0x00,  0x00,
                                                0x00, 0x00,  0x00,
                                                0x00, 0x00,  0x00,
                                                0x00, 0x00,  0x00,
                                                0x00, 0x00,  0x00,
                                                0x00, 0x00,  0x00,
                                                0x00, 0x00,  0x00,
                                                0x00, 0x00,  0x00,
                                                0x00, 0x00,  0x00,
                                                0x00};

  SceUsbdDeviceRequest _dr;
  _dr.bmRequestType = 0x21;
  _dr.bRequest      = HID_SET_REPORT;
  _dr.wValue        = (HID_REPORT_TYPE_OUTPUT << 8) | 0x01;
  _dr.wIndex        = 0;
  _dr.wLength       = sizeof(cmd);
  int ret           = ksceUsbdControlTransfer(c->pipe_control, (&_dr), (uint8_t *)&cmd, NULL, NULL);
}

void DS3Controller_setLed(Controller *c, uint8_t led)
{
  uint8_t cmd[] __attribute__((aligned(64)))
  = {0x00, 0x00, 0x00, 0x00, 0x00,        // rumble values
     0x00, 0x00, 0x00, 0x00, 0x01 << led, // 0x10=LED1 .. 0x02=LED4
     0xff, 0x27, 0x10, 0x00, 0x32,        // LED 4
     0xff, 0x27, 0x10, 0x00, 0x32,        // LED 3
     0xff, 0x27, 0x10, 0x00, 0x32,        // LED 2
     0xff, 0x27, 0x10, 0x00, 0x32,        // LED 1
     0x00, 0x00, 0x00, 0x00, 0x00,        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00,        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  SceUsbdDeviceRequest _dr;
  _dr.bmRequestType = 0x21;
  _dr.bRequest      = HID_SET_REPORT;
  _dr.wValue        = (HID_REPORT_TYPE_OUTPUT << 8) | 0x01;
  _dr.wIndex        = 0;
  _dr.wLength       = sizeof(cmd);
  ksceUsbdControlTransfer(c->pipe_control, (&_dr), (uint8_t *)&cmd, &oncontrol, NULL);
}

uint8_t DS3Controller_processReport(Controller *c, size_t length)
{
  DS3Report report;

  report.select = bit(c->buffer + 2, 0);
  report.l3     = bit(c->buffer + 2, 1);
  report.r3     = bit(c->buffer + 2, 2);
  report.start  = bit(c->buffer + 2, 3);

  report.dpad_up    = bit(c->buffer + 2, 4);
  report.dpad_right = bit(c->buffer + 2, 5);
  report.dpad_down  = bit(c->buffer + 2, 6);
  report.dpad_left  = bit(c->buffer + 2, 7);

  report.l2 = bit(c->buffer + 3, 0);
  report.r2 = bit(c->buffer + 3, 1);
  report.l1 = bit(c->buffer + 3, 2);
  report.r1 = bit(c->buffer + 3, 3);

  report.triangle = bit(c->buffer + 3, 4);
  report.circle   = bit(c->buffer + 3, 5);
  report.cross    = bit(c->buffer + 3, 6);
  report.square   = bit(c->buffer + 3, 7);

  report.ps = bit(c->buffer + 4, 0);

  report.x1 = c->buffer[6];
  report.y1 = c->buffer[7];
  report.x2 = c->buffer[8];
  report.y2 = c->buffer[9];

  report.l2a = c->buffer[18];
  report.r2a = c->buffer[19];

  c->controlData.buttons = 0;

  if (report.cross)
    c->controlData.buttons |= SCE_CTRL_CROSS;
  if (report.circle)
    c->controlData.buttons |= SCE_CTRL_CIRCLE;
  if (report.triangle)
    c->controlData.buttons |= SCE_CTRL_TRIANGLE;
  if (report.square)
    c->controlData.buttons |= SCE_CTRL_SQUARE;

  if (report.dpad_up)
    c->controlData.buttons |= SCE_CTRL_UP;
  if (report.dpad_down)
    c->controlData.buttons |= SCE_CTRL_DOWN;
  if (report.dpad_left)
    c->controlData.buttons |= SCE_CTRL_LEFT;
  if (report.dpad_right)
    c->controlData.buttons |= SCE_CTRL_RIGHT;

  if (report.l1)
    c->controlData.buttons |= SCE_CTRL_L1;
  if (report.r1)
    c->controlData.buttons |= SCE_CTRL_R1;
  if (report.l3)
    c->controlData.buttons |= SCE_CTRL_L3;
  if (report.r3)
    c->controlData.buttons |= SCE_CTRL_R3;

  if (report.l2)
    c->controlData.buttons |= SCE_CTRL_LTRIGGER;
  if (report.r2)
    c->controlData.buttons |= SCE_CTRL_RTRIGGER;

  if (report.start)
    c->controlData.buttons |= SCE_CTRL_START;
  if (report.select)
    c->controlData.buttons |= SCE_CTRL_SELECT;
  if (report.ps)
    c->controlData.buttons |= SCE_CTRL_PSBUTTON;

  c->controlData.leftX  = report.x1; // / 256 + 128;
  c->controlData.leftY  = report.y1; // / 256 + 128;
  c->controlData.rightX = report.x2; // / 256 + 128;
  c->controlData.rightY = report.y2; // / 256 + 128;

  c->controlData.lt = report.l2a;
  c->controlData.rt = report.r2a;
  return 1;
}

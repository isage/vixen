#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "devicelist.h"

#include <psp2common/types.h>
#include <psp2kern/kernel/debug.h>
#include <psp2kern/usbd.h>
#include <stdint.h>

typedef struct
{
  uint32_t buttons;
  uint8_t leftX;
  uint8_t leftY;
  uint8_t rightX;
  uint8_t rightY;
  uint8_t lt;
  uint8_t rt;
} ControlData;

typedef struct
{
  uint8_t type;
  uint8_t attached; // actual gamepad attached
  uint8_t inited;   // usb device attached and inited
  uint8_t battery_level;
  ControlData controlData;
  int device_id;
  uint8_t port;
  SceUID pipe_in;
  SceUID pipe_out;
  SceUID pipe_control;
  unsigned char buffer[64] __attribute__((aligned(64)));
  size_t buffer_size;

} Controller;

void usb_read(Controller *c);
void usb_write(Controller *c, uint8_t *data, int len);

#endif // __CONTROLLER_H__

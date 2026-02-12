#ifndef __DEVICELIST_H__
#define __DEVICELIST_H__

#include <stdint.h>
#include "controller.h"

typedef enum
{
  PAD_UNKNOWN,
  PAD_XBOX,
  PAD_XBOX360,
  PAD_XBOX360W,
  PAD_DS3,
  PAD_DINPUT,
} vixenPadType;

typedef struct
{
  vixenPadType type;
  uint16_t idVendor;
  uint16_t idProduct;
  uint8_t (*processReport)(Controller *c, size_t length);
} gamepad_t;

extern gamepad_t _devices[];

#endif // __DEVICELIST_H__

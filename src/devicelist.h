#ifndef __DEVICELIST_H__
#define __DEVICELIST_H__

#include <stdint.h>

typedef enum
{
  PAD_UNKNOWN,
  PAD_XBOX360,
  PAD_XBOX360W
} vixenPadType;

typedef struct
{
  vixenPadType type;
  uint16_t idVendor;
  uint16_t idProduct;
  const char *name;
} gamepad_t;

extern gamepad_t _devices[];

#endif // __DEVICELIST_H__

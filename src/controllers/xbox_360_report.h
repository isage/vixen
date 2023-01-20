#ifndef __XBOX_360_REPORT_H__
#define __XBOX_360_REPORT_H__

#include <stdint.h>

typedef struct
{
  uint8_t dpad_up : 1;
  uint8_t dpad_down : 1;
  uint8_t dpad_left : 1;
  uint8_t dpad_right : 1;

  uint8_t start : 1;
  uint8_t back : 1;

  uint8_t thumb_l : 1;
  uint8_t thumb_r : 1;

  uint8_t lb : 1;
  uint8_t rb : 1;
  uint8_t guide : 1;

  uint8_t a : 1;
  uint8_t b : 1;
  uint8_t x : 1;
  uint8_t y : 1;

  uint8_t lt : 8;
  uint8_t rt : 8;

  uint16_t x1 : 16;
  uint16_t y1 : 16;

  uint16_t x2 : 16;
  uint16_t y2 : 16;
} Xbox360Report;

#endif // __XBOX_360_REPORT_H__

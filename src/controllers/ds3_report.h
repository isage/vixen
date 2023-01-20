#ifndef __DS3_REPORT_H__
#define __DS3_REPORT_H__

#include <stdint.h>

typedef struct
{
  uint8_t dpad_up : 1;
  uint8_t dpad_down : 1;
  uint8_t dpad_left : 1;
  uint8_t dpad_right : 1;

  uint8_t start : 1;
  uint8_t select : 1;

  uint8_t l3 : 1;
  uint8_t r3 : 1;

  uint8_t l1 : 1;
  uint8_t r1 : 1;
  uint8_t ps : 1;

  uint8_t cross : 1;
  uint8_t circle : 1;
  uint8_t square : 1;
  uint8_t triangle : 1;

  uint8_t l2 : 8;
  uint8_t r2 : 8;

  uint8_t l2a : 8;
  uint8_t r2a : 8;

  int16_t x1 : 16;
  int16_t y1 : 16;

  int16_t x2 : 16;
  int16_t y2 : 16;

} DS3Report;

#endif // __DS3_REPORT_H__

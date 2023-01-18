#ifndef __XBOX_360_REPORT_H__
#define __XBOX_360_REPORT_H__

#include "../controller.h"

typedef struct
{
  unsigned int dpad_up : 1;
  unsigned int dpad_down : 1;
  unsigned int dpad_left : 1;
  unsigned int dpad_right : 1;

  unsigned int start : 1;
  unsigned int back : 1;

  unsigned int thumb_l : 1;
  unsigned int thumb_r : 1;

  // data[3] ------------------
  unsigned int lb : 1;
  unsigned int rb : 1;
  unsigned int guide : 1;
  unsigned int dummy1 : 1;

  unsigned int a : 1;
  unsigned int b : 1;
  unsigned int x : 1;
  unsigned int y : 1;

  // data[4] ------------------
  unsigned int lt : 8;
  unsigned int rt : 8;

  // data[6] ------------------
  int x1 : 16;
  int y1 : 16;

  // data[10] -----------------
  int x2 : 16;
  int y2 : 16;

  // data[14]; ----------------
  unsigned int dummy2 : 32;
  unsigned int dummy3 : 16;
} Xbox360Report;

#endif // __XBOX_360_REPORT_H__

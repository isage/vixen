#ifndef __DINPUT_RAPHNETPSX_H__
#define __DINPUT_RAPHNETPSX_H__
#include <psp2kern/ctrl.h>

uint8_t raphnetpsx_processReport(Controller *c, size_t length)
{
  if (length >= 15)
  {
    // dpad
    if (bit(c->buffer + 14, 4))
      c->controlData.buttons |= SCE_CTRL_UP;
    if (bit(c->buffer + 14, 5))
      c->controlData.buttons |= SCE_CTRL_DOWN;
    if (bit(c->buffer + 14, 6))
      c->controlData.buttons |= SCE_CTRL_LEFT;
    if (bit(c->buffer + 14, 7))
      c->controlData.buttons |= SCE_CTRL_RIGHT;

    if (bit(c->buffer + 13, 0))
      c->controlData.buttons |= SCE_CTRL_SQUARE;
    if (bit(c->buffer + 13, 1))
      c->controlData.buttons |= SCE_CTRL_CROSS;
    if (bit(c->buffer + 13, 2))
      c->controlData.buttons |= SCE_CTRL_CIRCLE;
    if (bit(c->buffer + 13, 3))
      c->controlData.buttons |= SCE_CTRL_TRIANGLE;

    if (bit(c->buffer + 13, 4))
      c->controlData.buttons |= SCE_CTRL_START;
    if (bit(c->buffer + 13, 5))
      c->controlData.buttons |= SCE_CTRL_SELECT;

    if (bit(c->buffer + 13, 6))
      c->controlData.buttons |= SCE_CTRL_L1;
    if (bit(c->buffer + 13, 7))
      c->controlData.buttons |= SCE_CTRL_R1;

    if (bit(c->buffer + 14, 2))
      c->controlData.buttons |= SCE_CTRL_L3;
    if (bit(c->buffer + 14, 3))
      c->controlData.buttons |= SCE_CTRL_R3;

    // triggers
    int16_t ltx = *(int16_t*)(c->buffer + 11);
    c->controlData.lt = ((int32_t)ltx - 16000) * 255 / 16000;
    int16_t rtx = *(int16_t*)(c->buffer + 9);
    c->controlData.rt = ((int32_t)rtx - 16000) * 255 / 16000;

    // set buttons with little deadzone
    if (c->controlData.lt > 10)
      c->controlData.buttons |= SCE_CTRL_LTRIGGER;
    if (c->controlData.rt > 10)
      c->controlData.buttons |= SCE_CTRL_RTRIGGER;

    if (bit(c->buffer + 14, 0))
    {
      c->controlData.buttons |= SCE_CTRL_LTRIGGER;
      c->controlData.lt = 0xFF;
    }
    if (bit(c->buffer + 14, 1))
    {
      c->controlData.buttons |= SCE_CTRL_RTRIGGER;
      c->controlData.rt = 0xFF;
    }

    // axes
    int16_t lx = *(int16_t*)(c->buffer + 1);
    c->controlData.leftX = (int32_t)lx * 255 / 32000;
    int16_t ly = *(int16_t*)(c->buffer + 3);
    c->controlData.leftY = (int32_t)ly * 255 / 32000;

    int16_t rx = *(int16_t*)(c->buffer + 5);
    c->controlData.rightX = (int32_t)rx * 255 / 32000;
    int16_t ry = *(int16_t*)(c->buffer + 7);
    c->controlData.rightY = (int32_t)ry * 255 / 32000;

    if (bit(c->buffer + 13, 6) && bit(c->buffer + 13, 7) && bit(c->buffer + 13, 4)) // L+R+START combo
    {
      c->controlData.buttons |= SCE_CTRL_PSBUTTON;
      c->controlData.buttons &= ~SCE_CTRL_START;
    }
  }
  return 1;
}

#endif

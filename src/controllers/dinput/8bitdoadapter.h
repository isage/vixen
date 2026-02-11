#ifndef __DINPUT_8BITDOADAPTER_H__
#define __DINPUT_8BITDOADAPTER_H__
#include <psp2kern/ctrl.h>
#include "dpad_angle.h"

uint8_t eightbitdoadapter_processReport(Controller *c, size_t length)
{
    process_dpad_angle(c, 3);

    if (bit(c->buffer + 1, 0))
      c->controlData.buttons |= SCE_CTRL_CROSS;
    if (bit(c->buffer + 1, 1))
      c->controlData.buttons |= SCE_CTRL_CIRCLE;
    if (bit(c->buffer + 1, 3))
      c->controlData.buttons |= SCE_CTRL_SQUARE;
    if (bit(c->buffer + 1, 4))
      c->controlData.buttons |= SCE_CTRL_TRIANGLE;

    if (bit(c->buffer + 1, 6))
      c->controlData.buttons |= SCE_CTRL_L1;
    if (bit(c->buffer + 1, 7))
      c->controlData.buttons |= SCE_CTRL_R1;

    if (bit(c->buffer + 2, 5))
      c->controlData.buttons |= SCE_CTRL_L3;
    if (bit(c->buffer + 2, 6))
      c->controlData.buttons |= SCE_CTRL_R3;

    if (bit(c->buffer + 2, 0))
    {
      c->controlData.buttons |= SCE_CTRL_LTRIGGER;
      c->controlData.lt = c->buffer[9];
    }

    if (bit(c->buffer + 2, 1))
    {
      c->controlData.buttons |= SCE_CTRL_RTRIGGER;
      c->controlData.rt = c->buffer[8];
    }

    if (bit(c->buffer + 2, 3))
      c->controlData.buttons |= SCE_CTRL_START;
    if (bit(c->buffer + 2, 2))
      c->controlData.buttons |= SCE_CTRL_SELECT;

    if (bit(c->buffer + 2, 4))
      c->controlData.buttons |= SCE_CTRL_PSBUTTON;

    c->controlData.leftX  = c->buffer[4];
    c->controlData.leftY  = c->buffer[5];

    c->controlData.rightX = c->buffer[6];
    c->controlData.rightY = c->buffer[7];

    return 1;
}

#endif

#ifndef __DINPUT_NEOGEOX_H__
#define __DINPUT_NEOGEOX_H__
#include <psp2kern/ctrl.h>
#include "dpad_angle.h"

uint8_t neogeox_processReport(Controller *c, size_t length)
{
    process_dpad_angle(c, 2);

    if (bit(c->buffer, 0))
      c->controlData.buttons |= SCE_CTRL_CROSS;
    if (bit(c->buffer, 1))
      c->controlData.buttons |= SCE_CTRL_CIRCLE;
    if (bit(c->buffer, 2))
      c->controlData.buttons |= SCE_CTRL_SQUARE;
    if (bit(c->buffer, 3))
      c->controlData.buttons |= SCE_CTRL_TRIANGLE;


    if (bit(c->buffer + 1, 0))
      c->controlData.buttons |= SCE_CTRL_SELECT;
    if (bit(c->buffer + 1, 1))
      c->controlData.buttons |= SCE_CTRL_START;

    return 1;
}

#endif

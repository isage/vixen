#ifndef __DINPUT_SMARTJOYPAD_H__
#define __DINPUT_SMARTJOYPAD_H__
#include <psp2kern/ctrl.h>

uint8_t smartjoypad_processReport(Controller *c, size_t length)
{

    if (bit(c->buffer + 1, 4))
      c->controlData.buttons |= SCE_CTRL_UP;
    if (bit(c->buffer + 1, 5))
      c->controlData.buttons |= SCE_CTRL_RIGHT;
    if (bit(c->buffer + 1, 6))
      c->controlData.buttons |= SCE_CTRL_DOWN;
    if (bit(c->buffer + 1, 7))
      c->controlData.buttons |= SCE_CTRL_LEFT;


    if (bit(c->buffer, 0))
      c->controlData.buttons |= SCE_CTRL_TRIANGLE;
    if (bit(c->buffer, 1))
      c->controlData.buttons |= SCE_CTRL_CIRCLE;
    if (bit(c->buffer, 2))
      c->controlData.buttons |= SCE_CTRL_CROSS;
    if (bit(c->buffer, 3))
      c->controlData.buttons |= SCE_CTRL_SQUARE;


    if (bit(c->buffer, 6))
      c->controlData.buttons |= SCE_CTRL_L1;
    if (bit(c->buffer, 7))
      c->controlData.buttons |= SCE_CTRL_R1;

    if (bit(c->buffer, 4))
    {
      c->controlData.buttons |= SCE_CTRL_LTRIGGER;
      c->controlData.lt = 0xFF;
    }

    if (bit(c->buffer, 5))
    {
      c->controlData.buttons |= SCE_CTRL_RTRIGGER;
      c->controlData.rt = 0xFF;
    }

    if (bit(c->buffer + 1, 2))
      c->controlData.buttons |= SCE_CTRL_L3;
    if (bit(c->buffer + 1, 3))
      c->controlData.buttons |= SCE_CTRL_R3;

    if (bit(c->buffer + 1, 0))
      c->controlData.buttons |= SCE_CTRL_SELECT;
    if (bit(c->buffer + 1, 1))
      c->controlData.buttons |= SCE_CTRL_START;

    if (bit(c->buffer, 6) && bit(c->buffer, 7) && bit(c->buffer + 1, 1)) // L+R+START combo
    {
      c->controlData.buttons |= SCE_CTRL_PSBUTTON;
      c->controlData.buttons &= ~SCE_CTRL_START;
    }

    c->controlData.leftX  = c->buffer[2];
    c->controlData.leftY  = c->buffer[3];

    c->controlData.rightX = c->buffer[4];
    c->controlData.rightY = c->buffer[5];

    return 1;
}

#endif

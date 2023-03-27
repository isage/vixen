#ifndef __DINPUT_PSCLASSIC_H__
#define __DINPUT_PSCLASSIC_H__
#include <psp2kern/ctrl.h>

uint8_t psclassic_processReport(Controller *c, size_t length)
{
    uint8_t dpadx = (c->buffer[1] >> 2) & 0x03;
    uint8_t dpady = (c->buffer[1] >> 4) & 0x03;

    switch (dpadx)
    {
      case 0x0:
        c->controlData.buttons |= SCE_CTRL_LEFT;
        break;
      case 0x2:
        c->controlData.buttons |= SCE_CTRL_RIGHT;
        break;
      case 0x1:
      default:
        break;
    }

    switch (dpady)
    {
      case 0x0:
        c->controlData.buttons |= SCE_CTRL_UP;
        break;
      case 0x2:
        c->controlData.buttons |= SCE_CTRL_DOWN;
        break;
      case 0x1:
      default:
        break;
    }

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

    if (bit(c->buffer + 1, 1))
      c->controlData.buttons |= SCE_CTRL_START;
    if (bit(c->buffer + 1, 0))
      c->controlData.buttons |= SCE_CTRL_SELECT;

    if (bit(c->buffer, 6) && bit(c->buffer, 7) && bit(c->buffer + 1, 1)) // L+R+START combo
    {
      c->controlData.buttons |= SCE_CTRL_PSBUTTON;
      c->controlData.buttons &= ~SCE_CTRL_START;
    }

    return 1;
}

#endif

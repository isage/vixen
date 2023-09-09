#ifndef __DINPUT_LOGITECH_WINGMAN_H__
#define __DINPUT_LOGITECH_WINGMAN_H__
#include <psp2kern/ctrl.h>

uint8_t logitechWingman_processReport(Controller *c, size_t length)
{
    if (c->buffer[2] == 0x01)
      c->controlData.buttons |= SCE_CTRL_UP;
    if (c->buffer[2] == 0xFE)
      c->controlData.buttons |= SCE_CTRL_DOWN;

    if (c->buffer[1] == 0x01)
      c->controlData.buttons |= SCE_CTRL_LEFT;
    if (c->buffer[1] == 0xFE)
      c->controlData.buttons |= SCE_CTRL_RIGHT;

    if (bit(c->buffer, 0))
      c->controlData.buttons |= SCE_CTRL_CROSS;
    if (bit(c->buffer, 1))
      c->controlData.buttons |= SCE_CTRL_CIRCLE;
    if (bit(c->buffer, 2))
      c->controlData.buttons |= SCE_CTRL_SQUARE;
    if (bit(c->buffer, 3))
      c->controlData.buttons |= SCE_CTRL_TRIANGLE;

    return 1;
}

#endif

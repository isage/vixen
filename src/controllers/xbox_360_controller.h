#ifndef __XBOX_360_CONTROLLER_H__
#define __XBOX_360_CONTROLLER_H__

#include "../controller.h"

uint8_t Xbox360Controller_probe(Controller *c, int device_id, int port);
uint8_t Xbox360Controller_processReport(Controller *c, size_t length);
void Xbox360Controller_setRumble(Controller *c, uint8_t small, uint8_t large);

#endif // __XBOX_360_CONTROLLER_H__

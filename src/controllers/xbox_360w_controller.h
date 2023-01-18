#ifndef __XBOX_360W_CONTROLLER_H__
#define __XBOX_360W_CONTROLLER_H__

#include "../controller.h"

uint8_t Xbox360WController_probe(Controller *c, int device_id, int port);
uint8_t Xbox360WController_processReport(Controller *c, size_t length);
void Xbox360WController_setLed(Controller *c, uint8_t led);
void Xbox360WController_setRumble(Controller *c, uint8_t small, uint8_t large);
void Xbox360WController_turnOff(Controller *c);

#endif // __XBOX_360W_CONTROLLER_H__

#ifndef __DS3_CONTROLLER_H__
#define __DS3_CONTROLLER_H__

#include "../controller.h"

uint8_t DS3Controller_probe(Controller *c, int device_id, int port);
uint8_t DS3Controller_processReport(Controller *c, size_t length);
void DS3Controller_setRumble(Controller *c, uint8_t small, uint8_t large);
void DS3Controller_setLed(Controller *c, uint8_t led);

#endif // __DS3_CONTROLLER_H__

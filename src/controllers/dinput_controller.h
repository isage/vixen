#ifndef __DINPUT_CONTROLLER_H__
#define __DINPUT_CONTROLLER_H__

#include "../controller.h"

uint8_t DinputController_probe(Controller *c, int device_id, int port, int vendor, int product);
uint8_t DinputController_processReport(Controller *c, size_t length);
void DinputController_setRumble(Controller *c, uint8_t small, uint8_t large);

#endif // __DINPUT_CONTROLLER_H__

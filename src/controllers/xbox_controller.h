#ifndef __XBOX_CONTROLLER_H__
#define __XBOX_CONTROLLER_H__

#include "../controller.h"

uint8_t XboxController_probe(Controller *c, int device_id, int port);
uint8_t XboxController_processReport(Controller *c, size_t length);
void XboxController_setRumble(Controller *c, uint8_t small, uint8_t large);

#endif // __XBOX_CONTROLLER_H__

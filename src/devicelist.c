#include "devicelist.h"

#include "controllers/utils/bit.h"
#include "controllers/dinput/logitech.h"
#include "controllers/dinput/logitech_wingman.h"
#include "controllers/dinput/psclassic.h"
#include "controllers/dinput/horidiva.h"
#include "controllers/dinput/horidiva_ps4.h"
#include "controllers/dinput/raphnetpsx.h"
#include "controllers/dinput/p3converter.h"
#include "controllers/dinput/sfcconverter.h"
#include "controllers/dinput/p2top3converter.h"
#include "controllers/dinput/smartjoypad.h"
#include "controllers/dinput/mayflash.h"
#include "controllers/dinput/neogeox.h"
#include "controllers/dinput/8bitdoadapter.h"

gamepad_t _devices[] = {{PAD_XBOX360, 0x045e, 0x028e, NULL},  // Microsoft X-Box 360 pad
                        {PAD_XBOX360, 0x045e, 0x028f, NULL},  // Microsoft X-Box 360 pad v2
                        {PAD_XBOX360, 0x0738, 0x4716, NULL},  // Mad Catz Wired Xbox 360 Controller
                        {PAD_XBOX360, 0x0738, 0x4726, NULL},  // Mad Catz Xbox 360 Controller
                        {PAD_XBOX360, 0x0738, 0x4728, NULL},  // Mad Catz Street Fighter IV FightPad
                        {PAD_XBOX360, 0x0738, 0x4740, NULL},  // Mad Catz Beat Pad
                        {PAD_XBOX360, 0x0738, 0xb726, NULL},  // Mad Catz Xbox controller - MW2
                        {PAD_XBOX360, 0x0738, 0xf738, NULL},  // Super SFIV FightStick TE S
                        {PAD_XBOX360, 0x0738, 0x4718, NULL},  // Mad Catz Street Fighter IV FightStick SE
                        {PAD_XBOX360, 0x0738, 0x4738, NULL},  // Mad Catz Wired Xbox 360 Controller (SFIV)
                        {PAD_XBOX360, 0x0738, 0xbeef, NULL},  // Mad Catz JOYTECH NEO SE Advanced GamePad
                        {PAD_XBOX360, 0x0f0d, 0x000a, NULL},  // Hori Co. DOA4 FightStick
                        {PAD_XBOX360, 0x0f0d, 0x000d, NULL},  // Hori Fighting Stick EX2
                        {PAD_XBOX360, 0x0f0d, 0x0016, NULL},  // Hori Real Arcade Pro.EX
                        {PAD_XBOX360, 0x056e, 0x2004, NULL},  // Elecom JC-U3613M
                        {PAD_XBOX360, 0x24c6, 0x5501, NULL},  // Hori Real Arcade Pro VX-SA
                        {PAD_XBOX360, 0x24c6, 0x5303, NULL},  // Xbox Airflo wired controller
                        {PAD_XBOX360, 0x24c6, 0x531a, NULL},  // PowerA Pro Ex
                        {PAD_XBOX360, 0x24c6, 0x5397, NULL},  // FUS1ON Tournament Controller
                        {PAD_XBOX360, 0x24c6, 0x5503, NULL},  // Hori Fighting Edge
                        {PAD_XBOX360, 0x24c6, 0x550d, NULL},  // Hori GEM Xbox controller
                        {PAD_XBOX360, 0x24c6, 0x5b03, NULL},  // Thrustmaster Ferrari 458 Racing Wheel
                        {PAD_XBOX360, 0x162e, 0xbeef, NULL},  // Joytech Neo-Se Take2
                        {PAD_XBOX360, 0x044f, 0xb326, NULL},  // Thrustmaster Gamepad GP XID
                        {PAD_XBOX360, 0x046d, 0xc21d, NULL},  // Logitech Gamepad F310
                        {PAD_XBOX360, 0x046d, 0xc21e, NULL},  // Logitech Gamepad F510
                        {PAD_XBOX360, 0x046d, 0xc21f, NULL},  // Logitech Gamepad F710
                        {PAD_XBOX360, 0x046d, 0xc242, NULL},  // Logitech Chillstream Controller
                        {PAD_XBOX360, 0x0738, 0xcb03, NULL},  // Saitek P3200 Rumble Pad - PC/Xbox 360
                        {PAD_XBOX360, 0x0738, 0xcb02, NULL},  // Saitek Cyborg Rumble Pad - PC/Xbox 360
                        {PAD_XBOX360, 0x0e6f, 0x0201, NULL},  // Pelican PL-3601 'TSZ' Wired Xbox 360 Controller
                        {PAD_XBOX360, 0x0e6f, 0x0105, NULL},  // HSM3 Xbox360 dancepad
                        {PAD_XBOX360, 0x0e6f, 0x0113, NULL},  // Afterglow AX.1 Gamepad for Xbox 360
                        {PAD_XBOX360, 0x0e6f, 0x0413, NULL},  // Afterglow AX.1 Gamepad for Xbox 360
                        {PAD_XBOX360, 0x0e6f, 0x0213, NULL},  // Afterglow Gamepad for Xbox 360
                        {PAD_XBOX360, 0x0e6f, 0x0401, NULL},  // Logic3 Controller
                        {PAD_XBOX360, 0x0e6f, 0x0301, NULL},  // Logic3 Controller
                        {PAD_XBOX360, 0x12ab, 0x0301, NULL},  // PDP AFTERGLOW AX.1
                        {PAD_XBOX360, 0x1430, 0x4748, NULL},  // RedOctane Guitar Hero X-plorer
                        {PAD_XBOX360, 0x146b, 0x0601, NULL},  // BigBen Interactive XBOX 360 Controller
                        {PAD_XBOX360, 0x1bad, 0x0002, NULL},  // Harmonix Guitar for Xbox 360
                        {PAD_XBOX360, 0x1bad, 0x0003, NULL},  // Harmonix Drum Kit for Xbox 360
                        {PAD_XBOX360, 0x1bad, 0xf016, NULL},  // Mad Catz Xbox 360 Controller
                        {PAD_XBOX360, 0x1bad, 0xf018, NULL},  // Mad Catz Street Fighter IV SE Fighting Stick
                        {PAD_XBOX360, 0x1bad, 0xf021, NULL},  // Mad Cats Ghost Recon FS GamePad
                        {PAD_XBOX360, 0x1bad, 0xf023, NULL},  // MLG Pro Circuit Controller (Xbox)
                        {PAD_XBOX360, 0x1bad, 0xf028, NULL},  // Street Fighter IV FightPad
                        {PAD_XBOX360, 0x1bad, 0xf02e, NULL},  // Mad Catz Fightpad
                        {PAD_XBOX360, 0x1bad, 0xf038, NULL},  // Street Fighter IV FightStick TE
                        {PAD_XBOX360, 0x1bad, 0xf03a, NULL},  // Mad Catz SFxT Fightstick Pro
                        {PAD_XBOX360, 0x1bad, 0xf900, NULL},  // Harmonix Xbox 360 Controller
                        {PAD_XBOX360, 0x1bad, 0xf901, NULL},  // Gamestop Xbox 360 Controller
                        {PAD_XBOX360, 0x1bad, 0xf903, NULL},  // Tron Xbox 360 controller
                        {PAD_XBOX360, 0x1bad, 0xfa01, NULL},  // MadCatz GamePad
                        {PAD_XBOX360, 0x15e4, 0x3f00, NULL},  // Power A Mini Pro Elite
                        {PAD_XBOX360, 0x15e4, 0x3f10, NULL},  // Batarang Xbox 360 controller
                        {PAD_XBOX360, 0x24c6, 0x5000, NULL},  // Razer Atrox Arcade Stick
                        {PAD_XBOX360, 0x1689, 0xfd00, NULL},  // Razer Onza Tournament Edition
                        {PAD_XBOX360, 0x1689, 0xfd01, NULL},  // Razer Onza Classic Edition
                        {PAD_XBOX360, 0x1532, 0x0037, NULL},  // Razer Sabertooth
                        {PAD_XBOX360, 0x12ab, 0x0004, NULL},  // Honey Bee Xbox360 dancepad
                        {PAD_XBOX360, 0x15e4, 0x3f0a, NULL},  // Xbox Airflo wired controller
                        {PAD_XBOX360, 0x24c6, 0x5300, NULL},  // PowerA MINI PROEX Controller
                        {PAD_XBOX360, 0x24c6, 0x5500, NULL},  // Hori XBOX 360 EX 2 with Turbo
                        {PAD_XBOX360, 0x24c6, 0x5506, NULL},  // Hori SOULCALIBUR V Stick
                        {PAD_XBOX360, 0x24c6, 0x5b02, NULL},  // Thrustmaster, Inc. GPX Controller
                        {PAD_XBOX360, 0x24c6, 0x5d04, NULL},  // Razer Sabertooth
                        {PAD_XBOX360, 0x0e6f, 0x011f, NULL},  // Rock Candy Gamepad Wired Controller
                        {PAD_XBOX360, 0x0e6f, 0x021f, NULL},  // Rock Candy Gamepad for Xbox 360
                        {PAD_XBOX360, 0x045e, 0x02a1, NULL},  // Open-Frame1
                        {PAD_XBOX360, 0x2f24, 0x0087, NULL},  // Mayflash F500 V2 [XInput Mode]
                        {PAD_XBOX360W, 0x045e, 0x0719, NULL}, // Xbox 360 Wireless Receiver
                        {PAD_XBOX360W, 0x045e, 0x0291, NULL}, // Xbox 360 Wireless Receiver (xbox)

                        {PAD_XBOX, 0x0d2f, 0x0002, NULL},     // Andamiro Pump It Up pad
                        {PAD_XBOX, 0x045e, 0x0202, NULL},     // Microsoft X-Box pad v1 (US)
                        {PAD_XBOX, 0x045e, 0x0285, NULL},     // Microsoft X-Box pad (Japan)
                        {PAD_XBOX, 0x045e, 0x0287, NULL},     // Microsoft Xbox Controller S
                        {PAD_XBOX, 0x045e, 0x0289, NULL},     // Microsoft X-Box pad v2 (US)
                        {PAD_XBOX, 0x046d, 0xca84, NULL},     // Logitech Xbox Cordless Controller
                        {PAD_XBOX, 0x046d, 0xca88, NULL},     // Logitech Compact Controller for Xbox
                        {PAD_XBOX, 0x05fd, 0x1007, NULL},     // Mad Catz Controller (unverified)
                        {PAD_XBOX, 0x05fd, 0x107a, NULL},     // InterAct 'PowerPad Pro' X-Box pad (Germany)
                        {PAD_XBOX, 0x0738, 0x4516, NULL},     // Mad Catz Control Pad
                        {PAD_XBOX, 0x0738, 0x4522, NULL},     // Mad Catz LumiCON
                        {PAD_XBOX, 0x0738, 0x4526, NULL},     // Mad Catz Control Pad Pro
                        {PAD_XBOX, 0x0738, 0x4536, NULL},     // Mad Catz MicroCON
                        {PAD_XBOX, 0x0738, 0x4556, NULL},     // Mad Catz Lynx Wireless Controller
                        {PAD_XBOX, 0x0c12, 0x8802, NULL},     // Zeroplus Xbox Controller
                        {PAD_XBOX, 0x0c12, 0x8810, NULL},     // Zeroplus Xbox Controller
                        {PAD_XBOX, 0x0c12, 0x9902, NULL},     // HAMA VibraX - *FAULTY HARDWARE*
                        {PAD_XBOX, 0x0e4c, 0x1097, NULL},     // Radica Gamester Controller
                        {PAD_XBOX, 0x0e4c, 0x2390, NULL},     // Radica Games Jtech Controller
                        {PAD_XBOX, 0x0e6f, 0x0003, NULL},     // Logic3 Freebird wireless Controller
                        {PAD_XBOX, 0x0e6f, 0x0005, NULL},     // Eclipse wireless Controller
                        {PAD_XBOX, 0x0e6f, 0x0006, NULL},     // Edge wireless Controller
                        {PAD_XBOX, 0x0e8f, 0x0201, NULL},     // SmartJoy Frag Xpad/PS2 adaptor
                        {PAD_XBOX, 0x0f30, 0x0202, NULL},     // Joytech Advanced Controller
                        {PAD_XBOX, 0x0f30, 0x8888, NULL},     // BigBen XBMiniPad Controller
                        {PAD_XBOX, 0x102c, 0xff0c, NULL},     // Joytech Wireless Advanced Controller
                        {PAD_XBOX, 0x044f, 0x0f07, NULL},     // Thrustmaster, Inc. Controller
                        {PAD_XBOX, 0x0e8f, 0x3008, NULL},     // Generic xbox control (dealextreme)
#if defined(WITH_DS3)
                        {PAD_DS3, 0x054c, 0x0268, NULL}, // DS3 wired
#endif
                        {PAD_DINPUT, 0x046d, 0xc218, logitech_processReport},          // Logitech F510 Gamepad [DirectInput Mode] / Rumblepad 2
                        {PAD_DINPUT, 0x046d, 0xc216, logitech_processReport},          // Logitech F310 Gamepad [DirectInput Mode] / DualAction
                        {PAD_DINPUT, 0x046d, 0xc20c, logitechWingman_processReport},   // Logitech Wingman
                        {PAD_DINPUT, 0x054c, 0x0cda, psclassic_processReport},         // Playstation Classic
                        {PAD_DINPUT, 0x0f0d, 0x0049, horiDiva_processReport},          // Hori ps3 mini diva
                        {PAD_DINPUT, 0x0f0d, 0x00a6, horiDiva_processReport},          // Hori diva X ps3
                        {PAD_DINPUT, 0x0f0d, 0x00a5, horiDivaps4_processReport},       // Hori diva X ps4
                        {PAD_DINPUT, 0x0f0d, 0x0101, horiDivaps4_processReport},       // Hori diva FT DX ps4
                        {PAD_DINPUT, 0x0f0d, 0x0102, horiDiva_processReport},          // Hori diva FT DX ps3
                        {PAD_DINPUT, 0x289b, 0x0044, raphnetpsx_processReport},        // Raphnet Technologies PSX to USB v.1.0
                        {PAD_DINPUT, 0x0e8f, 0x0003, p3converter_processReport},       // PIII Converter Model: 538
                        {PAD_DINPUT, 0x0079, 0x1804, sfcconverter_processReport},      // NES/FC/SFC Joypad TO USB BOX
                        {PAD_DINPUT, 0x0810, 0x0001, p2top3converter_processReport},   // Dual PSX Adaptor
                        {PAD_DINPUT, 0x0810, 0x0003, p2top3converter_processReport},   // PS2 to PS3 USB

                        {PAD_DINPUT, 0x0f0d, 0x0022, horiDiva_processReport},          // Brook universal fighting board (ds3 mode, same as hori diva X ds3)
                        {PAD_DINPUT, 0x0c12, 0x0c30, horiDivaps4_processReport},       // Brook universal fighting board (ds4 mode, same as hori diva X ds4)

                        {PAD_DINPUT, 0x12ba, 0x0100, horiDiva_processReport},          // Raspberry Pi Pico GH
                        {PAD_DINPUT, 0x0f0d, 0x0092, horiDiva_processReport},          // Raspberry Pi Pico GP2040CE

                        {PAD_DINPUT, 0x0b43, 0x0001, smartjoypad_processReport},       // Smart Joypad 3 adapter
                        {PAD_DINPUT, 0x0925, 0x1700, mayflash_processReport},          // Mayflash SS
                        {PAD_DINPUT, 0x1292, 0x4e47, neogeox_processReport},           // NEOGEO X

                        {PAD_DINPUT, 0x2dc8, 0x3105, eightbitdoadapter_processReport}, // 8bitdo adapter 2 (dinput mode)

                        {PAD_UNKNOWN, 0x0000, 0x0000, NULL}}; // Null

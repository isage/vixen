#include "devicelist.h"

gamepad_t _devices[] = {{PAD_XBOX360, 0x045e, 0x028e},  // Microsoft X-Box 360 pad
                        {PAD_XBOX360, 0x045e, 0x028f},  // Microsoft X-Box 360 pad v2
                        {PAD_XBOX360, 0x0738, 0x4716},  // Mad Catz Wired Xbox 360 Controller
                        {PAD_XBOX360, 0x0738, 0x4726},  // Mad Catz Xbox 360 Controller
                        {PAD_XBOX360, 0x0738, 0x4728},  // Mad Catz Street Fighter IV FightPad
                        {PAD_XBOX360, 0x0738, 0x4740},  // Mad Catz Beat Pad
                        {PAD_XBOX360, 0x0738, 0xb726},  // Mad Catz Xbox controller - MW2
                        {PAD_XBOX360, 0x0738, 0xf738},  // Super SFIV FightStick TE S
                        {PAD_XBOX360, 0x0738, 0x4718},  // Mad Catz Street Fighter IV FightStick SE
                        {PAD_XBOX360, 0x0738, 0x4738},  // Mad Catz Wired Xbox 360 Controller (SFIV)
                        {PAD_XBOX360, 0x0738, 0xbeef},  // Mad Catz JOYTECH NEO SE Advanced GamePad
                        {PAD_XBOX360, 0x0f0d, 0x000a},  // Hori Co. DOA4 FightStick
                        {PAD_XBOX360, 0x0f0d, 0x000d},  // Hori Fighting Stick EX2
                        {PAD_XBOX360, 0x0f0d, 0x0016},  // Hori Real Arcade Pro.EX
                        {PAD_XBOX360, 0x056e, 0x2004},  // Elecom JC-U3613M
                        {PAD_XBOX360, 0x24c6, 0x5501},  // Hori Real Arcade Pro VX-SA
                        {PAD_XBOX360, 0x24c6, 0x5303},  // Xbox Airflo wired controller
                        {PAD_XBOX360, 0x24c6, 0x531a},  // PowerA Pro Ex
                        {PAD_XBOX360, 0x24c6, 0x5397},  // FUS1ON Tournament Controller
                        {PAD_XBOX360, 0x24c6, 0x5503},  // Hori Fighting Edge
                        {PAD_XBOX360, 0x24c6, 0x550d},  // Hori GEM Xbox controller
                        {PAD_XBOX360, 0x24c6, 0x5b03},  // Thrustmaster Ferrari 458 Racing Wheel
                        {PAD_XBOX360, 0x162e, 0xbeef},  // Joytech Neo-Se Take2
                        {PAD_XBOX360, 0x044f, 0xb326},  // Thrustmaster Gamepad GP XID
                        {PAD_XBOX360, 0x046d, 0xc21d},  // Logitech Gamepad F310
                        {PAD_XBOX360, 0x046d, 0xc21e},  // Logitech Gamepad F510
                        {PAD_XBOX360, 0x046d, 0xc21f},  // Logitech Gamepad F710
                        {PAD_XBOX360, 0x046d, 0xc242},  // Logitech Chillstream Controller
                        {PAD_XBOX360, 0x0738, 0xcb03},  // Saitek P3200 Rumble Pad - PC/Xbox 360
                        {PAD_XBOX360, 0x0738, 0xcb02},  // Saitek Cyborg Rumble Pad - PC/Xbox 360
                        {PAD_XBOX360, 0x0e6f, 0x0201},  // Pelican PL-3601 'TSZ' Wired Xbox 360 Controller
                        {PAD_XBOX360, 0x0e6f, 0x0105},  // HSM3 Xbox360 dancepad
                        {PAD_XBOX360, 0x0e6f, 0x0113},  // Afterglow AX.1 Gamepad for Xbox 360
                        {PAD_XBOX360, 0x0e6f, 0x0413},  // Afterglow AX.1 Gamepad for Xbox 360
                        {PAD_XBOX360, 0x0e6f, 0x0213},  // Afterglow Gamepad for Xbox 360
                        {PAD_XBOX360, 0x0e6f, 0x0401},  // Logic3 Controller
                        {PAD_XBOX360, 0x0e6f, 0x0301},  // Logic3 Controller
                        {PAD_XBOX360, 0x12ab, 0x0301},  // PDP AFTERGLOW AX.1
                        {PAD_XBOX360, 0x1430, 0x4748},  // RedOctane Guitar Hero X-plorer
                        {PAD_XBOX360, 0x146b, 0x0601},  // BigBen Interactive XBOX 360 Controller
                        {PAD_XBOX360, 0x1bad, 0x0002},  // Harmonix Guitar for Xbox 360
                        {PAD_XBOX360, 0x1bad, 0x0003},  // Harmonix Drum Kit for Xbox 360
                        {PAD_XBOX360, 0x1bad, 0xf016},  // Mad Catz Xbox 360 Controller
                        {PAD_XBOX360, 0x1bad, 0xf018},  // Mad Catz Street Fighter IV SE Fighting Stick
                        {PAD_XBOX360, 0x1bad, 0xf021},  // Mad Cats Ghost Recon FS GamePad
                        {PAD_XBOX360, 0x1bad, 0xf023},  // MLG Pro Circuit Controller (Xbox)
                        {PAD_XBOX360, 0x1bad, 0xf028},  // Street Fighter IV FightPad
                        {PAD_XBOX360, 0x1bad, 0xf02e},  // Mad Catz Fightpad
                        {PAD_XBOX360, 0x1bad, 0xf038},  // Street Fighter IV FightStick TE
                        {PAD_XBOX360, 0x1bad, 0xf03a},  // Mad Catz SFxT Fightstick Pro
                        {PAD_XBOX360, 0x1bad, 0xf900},  // Harmonix Xbox 360 Controller
                        {PAD_XBOX360, 0x1bad, 0xf901},  // Gamestop Xbox 360 Controller
                        {PAD_XBOX360, 0x1bad, 0xf903},  // Tron Xbox 360 controller
                        {PAD_XBOX360, 0x1bad, 0xfa01},  // MadCatz GamePad
                        {PAD_XBOX360, 0x15e4, 0x3f00},  // Power A Mini Pro Elite
                        {PAD_XBOX360, 0x15e4, 0x3f10},  // Batarang Xbox 360 controller
                        {PAD_XBOX360, 0x24c6, 0x5000},  // Razer Atrox Arcade Stick
                        {PAD_XBOX360, 0x1689, 0xfd00},  // Razer Onza Tournament Edition
                        {PAD_XBOX360, 0x1689, 0xfd01},  // Razer Onza Classic Edition
                        {PAD_XBOX360, 0x1532, 0x0037},  // Razer Sabertooth
                        {PAD_XBOX360, 0x12ab, 0x0004},  // Honey Bee Xbox360 dancepad
                        {PAD_XBOX360, 0x15e4, 0x3f0a},  // Xbox Airflo wired controller
                        {PAD_XBOX360, 0x24c6, 0x5300},  // PowerA MINI PROEX Controller
                        {PAD_XBOX360, 0x24c6, 0x5500},  // Hori XBOX 360 EX 2 with Turbo
                        {PAD_XBOX360, 0x24c6, 0x5506},  // Hori SOULCALIBUR V Stick
                        {PAD_XBOX360, 0x24c6, 0x5b02},  // Thrustmaster, Inc. GPX Controller
                        {PAD_XBOX360, 0x24c6, 0x5d04},  // Razer Sabertooth
                        {PAD_XBOX360, 0x0e6f, 0x011f},  // Rock Candy Gamepad Wired Controller
                        {PAD_XBOX360, 0x0e6f, 0x021f},  // Rock Candy Gamepad for Xbox 360
                        {PAD_XBOX360W, 0x045e, 0x0719}, // Xbox 360 Wireless Receiver
                        {PAD_XBOX360W, 0x045e, 0x0291}, // Xbox 360 Wireless Receiver (xbox)
                        {PAD_XBOX360, 0x045e, 0x02a1}, // Open-Frame1

                        {PAD_XBOX, 0x0d2f, 0x0002},     // Andamiro Pump It Up pad
                        {PAD_XBOX, 0x045e, 0x0202},     // Microsoft X-Box pad v1 (US)
                        {PAD_XBOX, 0x045e, 0x0285},     // Microsoft X-Box pad (Japan)
                        {PAD_XBOX, 0x045e, 0x0287},     // Microsoft Xbox Controller S
                        {PAD_XBOX, 0x045e, 0x0289},     // Microsoft X-Box pad v2 (US)
                        {PAD_XBOX, 0x046d, 0xca84},     // Logitech Xbox Cordless Controller
                        {PAD_XBOX, 0x046d, 0xca88},     // Logitech Compact Controller for Xbox
                        {PAD_XBOX, 0x05fd, 0x1007},     // Mad Catz Controller (unverified)
                        {PAD_XBOX, 0x05fd, 0x107a},     // InterAct 'PowerPad Pro' X-Box pad (Germany)
                        {PAD_XBOX, 0x0738, 0x4516},     // Mad Catz Control Pad
                        {PAD_XBOX, 0x0738, 0x4522},     // Mad Catz LumiCON
                        {PAD_XBOX, 0x0738, 0x4526},     // Mad Catz Control Pad Pro
                        {PAD_XBOX, 0x0738, 0x4536},     // Mad Catz MicroCON
                        {PAD_XBOX, 0x0738, 0x4556},     // Mad Catz Lynx Wireless Controller
                        {PAD_XBOX, 0x0c12, 0x8802},     // Zeroplus Xbox Controller
                        {PAD_XBOX, 0x0c12, 0x8810},     // Zeroplus Xbox Controller
                        {PAD_XBOX, 0x0c12, 0x9902},     // HAMA VibraX - *FAULTY HARDWARE*
                        {PAD_XBOX, 0x0e4c, 0x1097},     // Radica Gamester Controller
                        {PAD_XBOX, 0x0e4c, 0x2390},     // Radica Games Jtech Controller
                        {PAD_XBOX, 0x0e6f, 0x0003},     // Logic3 Freebird wireless Controller
                        {PAD_XBOX, 0x0e6f, 0x0005},     // Eclipse wireless Controller
                        {PAD_XBOX, 0x0e6f, 0x0006},     // Edge wireless Controller
                        {PAD_XBOX, 0x0e8f, 0x0201},     // SmartJoy Frag Xpad/PS2 adaptor
                        {PAD_XBOX, 0x0f30, 0x0202},     // Joytech Advanced Controller
                        {PAD_XBOX, 0x0f30, 0x8888},     // BigBen XBMiniPad Controller
                        {PAD_XBOX, 0x102c, 0xff0c},     // Joytech Wireless Advanced Controller
                        {PAD_XBOX, 0x044f, 0x0f07},     // Thrustmaster, Inc. Controller
                        {PAD_XBOX, 0x0e8f, 0x3008},     // Generic xbox control (dealextreme)
#if defined(WITH_DS3)
                        {PAD_DS3, 0x054c, 0x0268}, // DS3 wired
#endif
                        {PAD_DINPUT, 0x046d, 0xc218}, // Logitech F510 Gamepad [DirectInput Mode] / Rumblepad 2
                        {PAD_DINPUT, 0x046d, 0xc216}, // Logitech F310 Gamepad [DirectInput Mode] / DualAction
                        {PAD_DINPUT, 0x046d, 0xc20c}, // Logitech Wingman
                        {PAD_DINPUT, 0x054c, 0x0cda}, // Playstation Classic
                        {PAD_DINPUT, 0x0f0d, 0x0049}, // Hori ps3 mini diva
                        {PAD_DINPUT, 0x0f0d, 0x00a6}, // Hori diva X ps3
                        {PAD_DINPUT, 0x0f0d, 0x00a5}, // Hori diva X ps4
                        {PAD_DINPUT, 0x289b, 0x0044}, // Raphnet Technologies PSX to USB v.1.0
                        {PAD_DINPUT, 0x0e8f, 0x0003}, // PIII Converter Model: 538
                        {PAD_DINPUT, 0x0079, 0x1804}, // NES/FC/SFC Joypad TO USB BOX
                        {PAD_DINPUT, 0x0810, 0x0003}, // PS2 to PS3 USB

                        {PAD_DINPUT, 0x0f0d, 0x0022}, // Brook universal fighting board (ds3 mode, same as hori diva X ps3)
//                        {PAD_DINPUT, 0x0c12, 0x0c30}, // Brook universal fighting board (ds4 mode)

                        {PAD_DINPUT, 0x12ba, 0x0100}, // Raspberry Pi Pico GH


                        {PAD_UNKNOWN, 0x0000, 0x0000}}; // Null

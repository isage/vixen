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
                        {PAD_DS3, 0x054c, 0x0268},      // DS3 wired
                        {PAD_UNKNOWN, 0x0000, 0x0000}}; // Null

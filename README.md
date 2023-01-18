# ViXEn
**Vi**ta **X**-input **En**abler

PSVita kernel driver for x-input gamepads (e.g. xbox360)

## Features

* Support for up to 4 wired usb x-input devices.
* Support for up to 4 wireless Xbox 360 gamepads via 1 wired usb receiver.
* Auto turn-off wireless controllers on suspend.

Note: xbox 360 wireless receiver takes over all 4 gamepad ports, so you can't use wired and wireless gamepads simultaniously.

For full list of supported devices see [here](src/devicelist.c)

## Installing
* Copy `vixen.skprx` into `ur0:tai` folder
* Add `ur0:tai/vixen.skprx` line under `*KERNEL` in tai config and reboot.
* For vita you need usb Y-cable and external power. See [this](https://github.com/isage/vita-usb-ether#hardware) for example.

## Building

* Install vitausb from https://github.com/isage/vita-packages-extra
* `mkdir build && cmake -DCMAKE_BUILD_TYPE=Release .. && make`

## License

MIT, see LICENSE.md

## Credits

* [xboxdrv](https://github.com/xboxdrv/xboxdrv) - for vid/pid pairs and protocol description
* [xerpi](https://github.com/xerpi) - for ds3vita
* **CBPS discord** - for support and stupid ideas
* **rem** - for being lazy/buzy to do the same :P
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

## FAQ
* **Does it support joycon, xbox one s, _insert another wireless controller here_?**  
  NO. It supports **wired USB** controllers only. Yes, Xbox 360 is also wired (because you need receiver)
  For wireless (bluetooth) use [ds34vita](https://github.com/MERLev/ds34vita), [VitaControl](https://github.com/Hydr8gon/VitaControl) or [MiniVitaTV](https://github.com/TheOfficialFloW/MiniVitaTV)
* **Can i connect mu Xbox 360 pad via charge'n'play?**  
  No. Because it's more charge than play. It doesn't support data. You need wireless receiver.
* **Does it support _insert controller name here_?**  
  Currently it supports plenty of x-input devices and wired ds3 (or ds3 compatible). See [here](src/devicelist.c)
  If your device isn't in that list (or doesn't present itself as ds3 or xbox360 pad), see [that list](https://github.com/xboxdrv/xboxdrv/blob/stable/src/xpad_device.cpp#L29)
  If it's in that list - i can add support. If it isn't - i, most likely, can't without having such device.
* **Can i install it alongside another input plugin?**
  Generally that isn't recommended. Most input plugins (ds34vita/vitacontrol) hook same functions, conflicts will definitely arise.

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
* [Graphene](https://github.com/GrapheneCt) - for testing and ideas
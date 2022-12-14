# Tiny Osean

Minimal PCB badge based around the ATTiny1616 and a CR2032 battery.

![Board in action](/imgs/demo.gif)

This PCB mainly focuses on the capabilities of the manufacturing process to create a non standard PCB with more intricate shapes and patterns. At the end of the day this is more of a wearable/toy PCB rather than a complex application/platform.

DISCLAIMER: I'm not the creator of the artwork, [@oseanworld](https://twitter.com/OseanWorld) is the artist behind the character in the badge.

## Hardware

The board is pretty basic. In total there are 16 components:

- 1 x ATTiny1616: Low power microcontroller.
- 1 x 100nF bypass capacitor.
- 1 x 1uF bypass capacitor.
- 1 x push button.
- 1 x 10k ohm resistor
- 8 x 10 ohm resistors.
- 2 x yellow LEDs.
- 1 x CR2032 battery holder.

If you know what you're doing you could modify the design and add more LEDs, remove bypass caps (they aren't totally necessary with the ATTiny series), etc.

The board has three terminals, two of them are to give power to the board (the _+_ and _-_ pins) and the _D_ terminal is connected to the _PA0_ pin on the ATTiny. This enables the device to be programmed via UPDI.

The design was done through the [svg2shenzhen](https://github.com/badgeek/svg2shenzhen) addon for Inkscape. The _nudeki.svg_ file contains all the layers data for the board. The electrical and PCB design was done in [KiCad v6](https://www.kicad.org/).

Other than that, I think that's it. It's a pretty bare bones design so make of it what you wish. Also the _gerber/_ and _assembly/_ directories contain the necessary files to order and assemble the boards at [jlcpcb.com](https://jlcpcb.com/).

One side note is that the LEDs are mounted in such a way that the light is emitted from the board. The ideal way to mount them is facing down so that the light can shine through the FR-4 and illuminate the eyes more uniformly. With the right tools it's possible to unmount the LEDs and reverse them.

## Firmware

The firmware of the board is located under _firmware/main/_. It contains a simple Arduino sketch but it has the following dependencies:

- [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore): Arduino wrapper for tinyAVR 0/1/2 chips.
- [jtag2updi](https://github.com/SpenceKonde/jtag2updi): UPDI programmer software for Arduino.

Install these dependencies and you'll be able to use an Arduino UNO or similar boards as programmers for the _tiny-osean_ board. If you have any problem with the installation procedure or wiring try looking at [this video](https://www.youtube.com/watch?v=AL9vK_xMt4E).

The _main.ino_ file contains a handful of lighting modes for the board and tries to document them as needed. The programming of the LEDs is pretty straight forward and user friendly, just beware of your logic inside the functions so that the interrupt signal gets processed in a relatively fast way to reduce latency.

## Design Images

### Front View

![3D Front View](/imgs/front.PNG)

### Back View

![3D Back View](/imgs/back.PNG)

### Layers

![Layers](/imgs/layers.PNG)

### Schematic

![Schematic](/imgs/schematic.PNG)

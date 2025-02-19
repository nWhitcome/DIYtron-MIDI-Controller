﻿# DIYtron MIDI Controller
This repo is a part of my hardware aspect to my ongoing project to build a Mellotron emulation.
It takes inputs from five potentiometers and two rotary encoders into an Arduino Pro Micro, translating values into MIDI messages that are then sent to an Intel Compute stick that is hosting my Mellotron software emulation.

## Build / Deploy
This code is built for an Arduino, so it needs to be imported into the Arduino IDE and sent to a connected board. It should work as long as the processor is an ATmega32U4 or something similar.

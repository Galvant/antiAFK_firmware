antiAFK Firmware
================

Arduino code for antiAFK project

About
-----

This code turns an Arduino Leonardo-compatible board into a USB keyboard which provides periodic
random keypresses to the attached computer. The time between key press events are defined by two
parameters, period and variance, such that period +- random(variance) is when the next key will
be pressed. This value is generated down to 1 millisecond resolution. Both period and variance
can be changed by the user and is preserved through power loss.

Once the appropriate amount of time has elapsed, a random key from a pre-defined set is sent to
PC. By default this is wasd and space. This key set can be changed by the user and is preserved 
through power loss.

Installation
------------

This can be installed on any Arduino Leonardo-compatible board using the official Arduion IDE 
typical installation methods.

Settings
--------

Period, variance, and the key set can all be changed by the user via the virtual serial port.
The easiest way for most will be to use the serial monitor built into the Arduino IDE.

period:10000

This will set the period to 10,000ms = 10sec. Period is stored in an unsigned long and thus
the max value is 2^(32)-1.

variance:5000

This sets the variance to 5,000ms = 5sec. Variance is stored in an unsigned long and thus the
max value is 2^(32)-1.

keys:w asd

This would set the valid keys to wasd and space.

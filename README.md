antiAFK Firmware
================

Arduino code for antiAFK project by Galvant Industries.

The associated PCB project can be found on GitHub at
www.github.com/Galvant/antiAFK-PCB

Pre-assembled boards can be found at http://galvant.ca/shop/antiafk/

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

Purpose
-------

The inspiration for this project came from my time playing MMORPGs. These games usually
involve a persistent online world that involve you logging into a server in 
order to play. Typically when a player reaches maximum level, these games
feature some sort of "end-game" group content, commonly referred to as 
dungeons, raids, or something else. Because of the group requirements, guilds
will schedule a time for everyone to login in order to tackle it.

Here is where the problem is. Some of these games are very popular can will
have a login queue. Sometimes this delay can be upwards of an hour for some
servers in games like World of Warcraft. This means you have to login well
ahead of time of the event , and also ensure that you do not get logged out.
Most of these online games will have a inactivity timeout period where you
are automatically logged out after a certain time of no input.

Imagine this situation. Your raid is coming up, but your server is so popular
that there is a 1 hour queue time. Thankfully you're already logged in, but
you want to get up away from the computer. Normally you would get logged out,
but instead by using the antiAFK you can stay logged in. Every few minutes
it sends a key press in such a way that it really looks like a person is
sitting there hitting a semi-random key on a USB keyboard every now-and-then.

This has the advantage over software solutions in that you are not running
code that might get you flagged for "botting". Many MMORPGs will scan system
memory to check for unauthorized software running. The antiAFK presents itself
to the PC as a completely legitimate HID keyboard, the same as your real USB 
keyboard does!

Usage
-----

Plug the antiAFK into an available USB port. On Windows, the first time you
do this you will get a device driver error. This is associated with the
virtual serial part and not the USB keyboard part. If you have no intention
on changing any settings you can safely ignore this.

After your PC is done acknowledging that you have plugged in the antiAFK
board, make sure that your PC game is the active window. Then, when you are
ready to activate it, press the push-button on the board. In order to provide 
some feedback, one of the valid keys will be immediately pressed.

You can now safely walk away from your game without worry that you will be 
automatically logged out. However, do note this will not save you from players
reporting you being away during player vs player matches!

Once you return, simply press the button again to deactivate the key presses.
Or, one could just remove the antiAFK from their PC USB port.

Compilation
-----------
To compile and upload this project, the following is required. Note that
pre-assembled boards already have the code loaded and thus this section
can be ignored if you have no intention of compiling the source code.

Required dependencies:
- Arduino Timer1 library http://playground.arduino.cc/code/timer1
- Entropy library https://code.google.com/p/avr-hardware-random-number-generation/wiki/WikiAVRentropy

This can be installed on any Arduino Leonardo-compatible board using the official Arduion IDE 
typical installation methods. 

When building your own, the push button is connected to digital pin 4 with a 
pull-up resistor. You can change this as you see fit by modifying the source
code.

Settings
--------

Period, variance, and the key set can all be changed by the user via the virtual serial port.
The easiest way for most will be to use the serial monitor built into the Arduino IDE. Most
users will find that the defaults are sufficient for most's needs.

`period:10000` This will set the period to 10,000ms = 10sec. Period is stored in an unsigned 
long and thus the max value is 2^(32)-1.

`variance:5000` This sets the variance to 5,000ms = 5sec. Variance is stored in an unsigned 
long and thus the max value is 2^(32)-1.

`keys:w asd` This would set the valid keys to wasd and space. Note that for this example
the space was located between the 'w' and 'a', but that is not required. It was simply 
done to help show that there is a space.

`toggle` This can be used to toggle the running state of the antiAFK board without 
having to activate the physical push-button.

`debug` This command will toggle on/off the sending of debug messages back to the PC over the 
virtual serial port. The default is off. This setting is not saved through power cycles.

`keyboard` This command will toggle on/off the actual keyboard key presses to the PC.
The default is on. This setting is not saved through power cycles.

License
-------

This code is released under the AGPLv3 license. A copy of the license, as well as author
information can be found in the LICENSE folder.

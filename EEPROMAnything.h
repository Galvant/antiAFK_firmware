/*
* This code is sourced from the Arduino website 
* http://playground.arduino.cc/Code/EEPROMWriteAnything
*
* Although the license is not explicitly stated on this webpage,
* I have made the assumption that it is released under the same
* license as the rest of the Arduino source code.
*/
#include <EEPROM.h>
#include <Arduino.h>  // for type definitions

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}

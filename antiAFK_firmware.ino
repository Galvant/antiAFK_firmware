/* antiAFK_firmware.ino: 
**
* © 2013 Steven Casagrande (scasagrande@galvant.ca).
*
* This file is a part of the InstrumentKit project.
* Licensed under the AGPL version 3.
**
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
**/

#include "TimerOne.h"
#include <EEPROM.h>
#include "EEPROMAnything.h"

const byte EEPROM_CODE = 0x5A;
const byte FIRMWARE_VERSION = 1;
const byte EEPROM_CODE_ADDRESS = 0x00;
const byte EEPROM_VERSION_ADDRESS = EEPROM_CODE_ADDRESS + 1; // 0x01
const byte EEPROM_PERIOD_ADDRESS = EEPROM_VERSION_ADDRESS + 1; // 0x02
const byte EEPROM_VARIANCE_ADDRESS = EEPROM_PERIOD_ADDRESS + 4; // 0x06
const byte EEPROM_VALID_KEYS_LENGTH_ADDRESS = EEPROM_VARIANCE_ADDRESS + 4; // 0xA0
const byte EEPROM_VALID_KEYS_ADDRESS = EEPROM_VALID_KEYS_LENGTH_ADDRESS + 1; // 0xA1

#define buttonPin 4

unsigned long period = 10*1000; // Delay between keyboard events in ms
unsigned long variance = 5*1000; // Maximum variance of period in ms
unsigned long duration = 20;
String valid_keys = "wasd ";
int valid_keys_length = valid_keys.length();
unsigned long nextKeyPress = period;

unsigned long counter = 0;
int prevButtonState = HIGH;
boolean running = false;
boolean debug = false;
boolean keyboard_enable = true;
byte eepromValue = 0;
String incomingCmd = "";
char nextKey = 0x00;

void setup() {
  Keyboard.begin();
  Serial.begin(9600);
  if (debug) {
    while(!Serial);
  }
  
  pinMode(buttonPin, INPUT);
  randomSeed(analogRead(0));
  
  Timer1.initialize(1000); // Starts timer with 1000us interupt
  Timer1.attachInterrupt(callback);
  
  //Check EEPROM for stored settings
  eepromValue = EEPROM.read(EEPROM_CODE_ADDRESS);
  if (eepromValue == EEPROM_CODE) {
    EEPROM_readAnything(EEPROM_PERIOD_ADDRESS, period);
    EEPROM_readAnything(EEPROM_VARIANCE_ADDRESS, variance);
    valid_keys_length = EEPROM.read(EEPROM_VALID_KEYS_LENGTH_ADDRESS);
    valid_keys = "";
    for (int i = 0; i < valid_keys_length; i++) {
      valid_keys += char(EEPROM.read(EEPROM_VALID_KEYS_ADDRESS + i));
    }
    if (debug) {
      Serial.println("Valid EEPROM code.");
      Serial.print("Period: ");
      Serial.println(period);
      Serial.print("Variance: ");
      Serial.println(variance);
    }
  }
  else { // EEPROM not valid, so initialize it
    EEPROM.write(EEPROM_CODE_ADDRESS, EEPROM_CODE);
    EEPROM.write(EEPROM_VERSION_ADDRESS, FIRMWARE_VERSION);
    EEPROM_writeAnything(EEPROM_PERIOD_ADDRESS, period);
    EEPROM_writeAnything(EEPROM_VARIANCE_ADDRESS, variance);
    valid_keys_length = valid_keys.length();
    EEPROM.write(EEPROM_VALID_KEYS_LENGTH_ADDRESS, valid_keys_length);
    for (int i = 0; i < valid_keys_length; i++) {
      EEPROM.write(EEPROM_VALID_KEYS_ADDRESS + i, valid_keys[i]);
    }
    if (debug) {
      Serial.println("Not valid EEPROM code.");
    }
  }
}

void callback() {
  if (running == true) {
    counter++;
    if (debug) {
      if (counter % 1000 == 0) {
        Serial.print("Counter: ");
        Serial.println(counter);
      }
    }
    if (counter >= nextKeyPress) {
      // Press the key
      nextKey = valid_keys[random(valid_keys.length())];
      if (debug) {
        Serial.print("Key press event after ");
        Serial.print(counter);
        Serial.println(" milliseconds.");
        Serial.print("Key pressed: ");
        if (nextKey == 32) {
          Serial.println("{space}");
        } else {
          Serial.println(nextKey);
        }
      }
      
      if (keyboard_enable) {
        Keyboard.press(nextKey);
        delay(duration);
        Keyboard.releaseAll();
      }
      
      // First, double check that variance is less than period
      // if not, load from EEPROM
      if (variance > period) {
        variance = EEPROM.read(EEPROM_VARIANCE_ADDRESS);
      }
      // Generate next key press time
      generateNextKeyPress();
    }
  }
}

void generateNextKeyPress() {
  /*
  * This method is used to determine the time until
  * the next key press event will occur.
  */
  if (random(0,2) == 0){
    nextKeyPress = period + random(0, variance + 1);
  } else {
    nextKeyPress = period - random(0, variance + 1);
  }
  duration = 20 + random(0,500);
  if (debug) {
    Serial.print("Next key press set to: ");
    Serial.println(nextKeyPress);
    Serial.print("Duration will be: ");
    Serial.println(duration);
  }
  counter = 0;
}

void toggleRunningState() {
  /*
  * Used to swap between running and standby modes of operation
  * This is called when the "toggle" string is send from the attached
  * PC, or the physical button is pushed.
  */
  running = !running;
  if (debug) {
    Serial.print("Running state switched to: ");
    Serial.println(running);
  }
  if (running) {
    counter = nextKeyPress; // Gives quick feedback that device is running
  }
  else {
    counter = 0;
  }
}

String readLine() {
  String value = "";
  for (int i = 0; Serial.available() > 0; i++) {
    value += (char)Serial.read();
    delay(5);
  }
  return value;
}

void loop() {
  // Get and parse messages from the attached PC
  if (Serial.available() > 0) {
    incomingCmd = readLine();
    //period:
    if (incomingCmd.substring(0,7).equalsIgnoreCase("period:")) {
      period = incomingCmd.substring(7,incomingCmd.length()).toInt();
      EEPROM_writeAnything(EEPROM_PERIOD_ADDRESS, period);
      generateNextKeyPress();
      if (debug) {
        Serial.print("Period set to: ");
        Serial.println(period);
      }
    }
    //variance:
    else if (incomingCmd.substring(0,9).equalsIgnoreCase("variance:")) {
      variance = incomingCmd.substring(9,incomingCmd.length()).toInt();
      if ((variance > period) || (variance < 0)){
        if (debug) {
          Serial.println("Variance invalid, loading from EEPROM.");
        }
        EEPROM_readAnything(EEPROM_VARIANCE_ADDRESS, variance);
      }
      else {
        EEPROM_writeAnything(EEPROM_VARIANCE_ADDRESS, variance);
        generateNextKeyPress();
      }
      if (debug) {
        Serial.print("Variance set to: ");
        Serial.println(variance);
      }
    }
    //toggle
    else if (incomingCmd.substring(0,6).equalsIgnoreCase("toggle")) {
      toggleRunningState();
    }
    //keys:
    else if (incomingCmd.substring(0,5).equalsIgnoreCase("keys:")) {
      if (incomingCmd.length() == 5) {
        if (debug) {
          Serial.println("Must specify at least 1 key.");
        }
      }
      else {
        valid_keys = incomingCmd.substring(5,incomingCmd.length());
        valid_keys_length = valid_keys.length();
        EEPROM.write(EEPROM_VALID_KEYS_LENGTH_ADDRESS, valid_keys_length);
        for (int i = 0; i < valid_keys_length; i++) {
          EEPROM.write(EEPROM_VALID_KEYS_ADDRESS + i, valid_keys[i]);
        }
      }
    }
    //debug
    else if (incomingCmd.substring(0,5).equalsIgnoreCase("debug")) {
      debug = !debug;
      if (debug) {
        Serial.println("Debug messages now enabled.");
      }
    }
    //keyboard
    else if (incomingCmd.substring(0,8).equalsIgnoreCase("keyboard")) {
      keyboard_enable = !keyboard_enable;
      if ((keyboard_enable) && (debug)) {
        Serial.println("Keyboard output enabled");
      }
      else if (debug) {
        Serial.println("Keyboard output disabled");
      }
    }
  }
  
  int buttonState = digitalRead(buttonPin);
  // Button is active low
  if ((buttonState != prevButtonState) && (buttonState == LOW)) {
    delay(100); // protect against switch debounce
    toggleRunningState();
  }
  else if ((buttonState != prevButtonState) && (buttonState == HIGH)) {
    delay(100); // protect against switch debounce
  }
  prevButtonState = buttonState;
  
}

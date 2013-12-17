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

//#define DEBUG 1
#define KEYBOARD_ENABLE 1

const byte EEPROM_CODE = 0x5A;
const byte EEPROM_CODE_ADDRESS = 0x00;
const byte EEPROM_VERSION_ADDRESS = 0x01;
const byte EEPROM_PERIOD_ADDRESS = 0x02;
const byte EEPROM_VARIANCE_ADDRESS = 0x03;
const byte EEPROM_VALID_KEYS_LENGTH_ADDRESS = 0x04;
const byte EEPROM_VALID_KEYS_ADDRESS = 0x05;

#define buttonPin 4

int period = 10; // Delay between keyboard events in seconds
int variance = 5; // Maximum variance of period in seconds
String valid_keys = "wasd ";
int valid_keys_length = valid_keys.length();
int nextKeyPress = period;

int counter = 0;
int prevButtonState = HIGH;
boolean running = false;
byte eepromValue = 0;
String incomingCmd = "";

void setup() {
  Keyboard.begin();
  Serial.begin(9600);
  #if defined(DEBUG)
  while(!Serial);
  #endif
  
  pinMode(buttonPin, INPUT);
  randomSeed(analogRead(0));
  
  Timer1.initialize(); // Starts timer with default 1sec interupt
  Timer1.attachInterrupt(callback);
  
  //Check EEPROM for stored settings
  eepromValue = EEPROM.read(EEPROM_CODE_ADDRESS);
  if (eepromValue == EEPROM_CODE) {
    period = EEPROM.read(EEPROM_PERIOD_ADDRESS);
    variance = EEPROM.read(EEPROM_VARIANCE_ADDRESS);
    valid_keys_length = EEPROM.read(EEPROM_VALID_KEYS_LENGTH_ADDRESS);
    valid_keys = "";
    for (int i = 0; i < valid_keys_length; i++) {
      valid_keys += char(EEPROM.read(EEPROM_VALID_KEYS_ADDRESS + i));
    }
    #if defined(DEBUG)
    Serial.println("Valid EEPROM code.");
    Serial.print("Period: ");
    Serial.println(period);
    Serial.print("Variance: ");
    Serial.println(variance);
    #endif
  }
  else { // EEPROM not valid, so initialize it
    EEPROM.write(EEPROM_CODE_ADDRESS, EEPROM_CODE);
    EEPROM.write(EEPROM_VERSION_ADDRESS, 1);
    EEPROM.write(EEPROM_PERIOD_ADDRESS, period);
    EEPROM.write(EEPROM_VARIANCE_ADDRESS, variance);
    valid_keys_length = valid_keys.length();
    EEPROM.write(EEPROM_VALID_KEYS_LENGTH_ADDRESS, valid_keys_length);
    for (int i = 0; i < valid_keys_length; i++) {
      EEPROM.write(EEPROM_VALID_KEYS_ADDRESS + i, valid_keys[i]);
    }
    #if defined(DEBUG)
    Serial.println("Not valid EEPROM code.");
    #endif
  }
}

void callback() {
  if (running == true) {
    counter++;
    #if defined(DEBUG)
    Serial.print("Counter: ");
    Serial.println(counter);
    #endif
    if (counter >= nextKeyPress) {
      // Press the key
      #if defined(DEBUG)
      Serial.print("Key press event after ");
      Serial.print(counter);
      Serial.println(" seconds.");
      Serial.print("Key pressed: ");
      Serial.println(valid_keys[random(valid_keys.length())]);
      #endif
      
      #if defined(KEYBOARD_ENABLE)
      Keyboard.print(valid_keys[random(valid_keys.length())]);
      #endif
      
      // First, double check that variance is less than period
      // if not, load from EEPROM
      if (variance > period) {
        variance = EEPROM.read(EEPROM_VARIANCE_ADDRESS);
      }
      // Generate next key press time
      if (random(0,2) == 0){
        nextKeyPress = period + random(0, variance + 1);
      } else {
        nextKeyPress = period - random(0, variance + 1);
      }
      // Reset seconds counter
      counter = 0;
    }
  }
}

void toggleRunningState() {
  running = !running;
  #if defined(DEBUG)
  Serial.print("Running state switched to: ");
  Serial.println(running);
  #endif
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
  if (Serial.available() > 0) {
    incomingCmd = readLine();
    if (incomingCmd.substring(0,7).equalsIgnoreCase("period:")) {
      period = incomingCmd.substring(7,incomingCmd.length()).toInt();
      EEPROM.write(EEPROM_PERIOD_ADDRESS, period);
      #if defined(DEBUG)
      Serial.print("Period set to: ");
      Serial.println(period);
      #endif
    }
    else if (incomingCmd.substring(0,9).equalsIgnoreCase("variance:")) {
      variance = incomingCmd.substring(9,incomingCmd.length()).toInt();
      if ((variance > period) || (variance < 0)){
        #if defined(DEBUG)
        Serial.println("Variance invalid, loading from EEPROM.");
        #endif
        variance = EEPROM.read(EEPROM_VARIANCE_ADDRESS);
      }
      else {
        EEPROM.write(EEPROM_VARIANCE_ADDRESS, variance);
      }
      #if defined(DEBUG)
      Serial.print("Variance set to: ");
      Serial.println(variance);
      #endif
    }
    else if (incomingCmd.substring(0,6).equalsIgnoreCase("toggle")) {
      toggleRunningState();
    }
    else if (incomingCmd.substring(0,5).equalsIgnoreCase("keys:")) {
      if (incomingCmd.length() == 5) {
        #if defined(DEBUG)
        Serial.println("Must specify at least 1 key.");
        #endif
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
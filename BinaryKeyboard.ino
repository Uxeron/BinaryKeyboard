/*
 * How the LED's are connected, the ones controlled from the arduino itself are controlled with a HIGH signal for ON, LOW for OFF, while the expander's LEDs are the opposite
 * This is because the expander cannot provide much current (up to around 40uA), so instead it acts as the ground
 */

#include <Keyboard.h>
#include "src/PCF8574.h"

PCF8574 expander;

int BYTE = 0; // Byte to be written to connected device
int pos = 0;  // Which bit is being written to

bool B_0 = false; // Button 0 is pressed
bool B_1 = false; // Button 1 is pressed
bool B_R = false; // Button Reset is pressed

int pins[] = {10, 16, 14, 15, 18, 19, 20, 7}; // Pin numbers for arduino controlled LEDs

void setup() {
  expander.begin(0x20);

  for(int i = 0; i < 8; i++) {
    // Setup arduino pins
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
    // Setup expander pins
    expander.pinMode(i, OUTPUT);
    expander.digitalWrite(i, HIGH);
  }
  // Setup key input pins
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
}


void clr() {
  // Set all pins to "Off" (LOW for arduino pins, HIGH for expander pins)
  for(int i = 0; i < 8; i++) {
    digitalWrite(pins[i], LOW);
    expander.digitalWrite(i, HIGH);
  }
  // Reset the byte and the bit position
  BYTE = 0;
  pos = 0;
}

void turnOn(int index, bool biit) {
  if (index > 3) { // Accessing LEDs connected directly to the arduino
    index -= 4;
    if (biit) digitalWrite(pins[(index)*2],     HIGH);  // Turn on the 1 LED
    else      digitalWrite(pins[(index)*2 + 1], HIGH);  // Turn on the 0 LED
  } else {
    if (biit) expander.digitalWrite((index)*2,     LOW);  // Turn on the 1 LED
    else      expander.digitalWrite((index)*2 + 1, LOW);  // Turn on the 0 LED
  }
}

void loop() {
  if(digitalRead(4) == 0) {
    if(!B_R) {
      clr();
      B_R = true;
    }
  } else B_R = false;
  
  if(digitalRead(5) == 0) {
    if(!B_1) {
      bitSet(BYTE, 7 - pos);
      turnOn(pos, true);
      pos++;
      B_1 = true;
    }
  } else B_1 = false;
  
  if(digitalRead(6) == 0) {
    if(!B_0) {
      // Don't set anyting, since the bit is already 0
      turnOn(pos, false);
      pos++;
      B_0 = true;
    }
  } else B_0 = false;

  if(pos == 8) {  // Write out the byte
    Keyboard.write(BYTE);
    delay(100);
    clr();
  }

  delay(10);
}

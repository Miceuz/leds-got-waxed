#include "DipConfig.h"
#include <SPI.h>
#include <Arduino.h>

DipConfig::DipConfig(uint8_t _latchPin, uint8_t _clkPin, uint8_t _dataPin):
	latchPin(_latchPin),
	clkPin(_clkPin),
	dataPin(_dataPin)
{
	pinMode(latchPin, OUTPUT);
	pinMode(clkPin, OUTPUT);
	pinMode(dataPin, INPUT);
}

byte shiftIn2(int myDataPin, int myClockPin) {
  int i;
  int temp = 0;
  int pinState;
  byte myDataIn = 0;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);
  for (i=7; i>=0; i--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(2);
    temp = digitalRead(myDataPin);
    if (temp) {
      pinState = 1;
      //set the bit to 0 no matter what
      myDataIn = myDataIn | (1 << i);
    }
    else {
      pinState = 0;
    }
    digitalWrite(myClockPin, 1);

  }
  return myDataIn;
}

uint16_t DipConfig::get() {
	digitalWrite(latchPin, LOW);
	delayMicroseconds(5);
	digitalWrite(latchPin, HIGH);

	digitalWrite(clkPin, LOW);

	uint8_t lower = ~shiftIn2(dataPin, clkPin);
	uint8_t higher = ~shiftIn2(dataPin, clkPin) & 0x0F;

	return (higher << 8) | lower;
}


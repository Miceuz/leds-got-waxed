#include "Effect.h"
#include "Tlc5940.h"
#include <Arduino.h>
#include "cie1931.h"

Effect::Effect(){
}

Effect::~Effect() {
	//NOTHING
}

uint8_t Effect::canInterrupt() {
	return 1;
}

uint8_t Effect::isFinished() {
	return 1;
}

void Effect::run(uint16_t newLevel, uint8_t firstOutput, uint8_t outputs){
}

FlashEffect::FlashEffect(){
	decay = 10;
}

uint8_t FlashEffect::canInterrupt() {
	return 0;
}

uint8_t FlashEffect::isFinished() {
	return curLevel == 0;
}

void FlashEffect::run(uint16_t newLevel, uint8_t firstOutput, uint8_t outputs) {
	if(isFinished() && newLevel > triggerLevel) {
		curLevel = newLevel;
	} else {
		if(curLevel >= decay) {
			curLevel -= decay;
		} else {
			curLevel = 0;
		}
	}

	for(uint8_t i = 0; i < outputs; i++) {
		Tlc.set(firstOutput + i, map(curLevel, 0, 1023, 0, 4095));			
	}
	Tlc.update();  
}


VUMeterEffect::VUMeterEffect() {
}

uint8_t VUMeterEffect::isFinished() {
	return curLevel > decay;
}

void VUMeterEffect::run(uint16_t newLevel, uint8_t firstOutput, uint8_t outputs) {
	// Serial.println(newLevel);
	if(newLevel >= curLevel) {
		curLevel = newLevel;
	} else if(curLevel >= decay) {
		curLevel-=decay;
	}

	Tlc.clear();

	//curLevel / (1024 / outputs)
	uint8_t outputsFullyOn = (curLevel * outputs) >> 10;

	// Serial.print("outputs:");
	// Serial.print(outputs);
	// Serial.print(" curLevel:");
	// Serial.print(curLevel);
	// Serial.print(" newLevel:");
	// Serial.print(newLevel);
	// Serial.print(" outputsFullyOn: ");
	// Serial.println(outputsFullyOn);


	for(int i = 0; i < outputsFullyOn; i++) {
		Tlc.set(firstOutput + i, 4095);
	}

	if(outputs != outputsFullyOn) {
		uint16_t lastOutput = map(curLevel - (outputsFullyOn * 1024 / outputs), 0, 1024 / outputs, 0, 4095);
		Tlc.set(outputsFullyOn, cie(lastOutput));			
	}

	Tlc.update();  
}
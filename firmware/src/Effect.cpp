#include "Effect.h"
#include "Tlc5940.h"
#include <Arduino.h>
#include "cie1931.h"

Effect::Effect(){

}

Effect::~Effect() {
	//NOTHING
}

void Effect::run(uint16_t newLevel, Output *output){
}

uint8_t Effect::isNotTriggerHoldOff() {
	return (millis() - lastBlink) > params.triggerHoldOff;
}


FlashEffect::FlashEffect(){
	id = 0;
	params.decay = 600;
	params.triggerLevel = 400;

	for(uint8_t i = 0; i < 16; i++) {
		levels[i] = 0;
		flags[i] = 0;
	}
}

void FlashEffect::run(uint16_t newLevel, Output *output) {
	newLevel = (long) newLevel * params.gain >> 10;

	if((newLevel > params.triggerLevel) && isNotTriggerHoldOff()) {
		curLevel = newLevel;
	} else {
		if(curLevel >= params.decay) {
			curLevel -= params.decay;
		} else {
			curLevel = 0;
		}
	}
	// Serial.print(" curLevel:");
	// Serial.println(curLevel);
	// Serial.print(" newLevel:");
	// Serial.println(newLevel);

	for(uint8_t i = 0; i < output->outputsInChannel; i++) {
		Tlc.set(output->firstOutput + i, cie(map(curLevel, 0, 1024, 0, params.maxBrightness)));			
	}
}


VUMeterEffect::VUMeterEffect() {
	id = 1;
	params.decay = 30;
	params.triggerLevel = 400;
	params.triggerHoldOff = 0;
	for(uint8_t i = 0; i < 16; i++) {
		levels[i] = 0;
		flags[i] = 0;
	}
}

void VUMeterEffect::run(uint16_t newLevel, Output *output) {
	newLevel = (long) newLevel * params.gain >> 10;

	if(newLevel >= curLevel && newLevel >= params.triggerLevel && isNotTriggerHoldOff()) {
		curLevel = newLevel;
		lastBlink = millis();
	} else {
		if(curLevel >= params.decay) {
			curLevel -= params.decay;
		} else {
			curLevel = 0;
		}
	}


	//curLevel / (1024 / outputs)
	uint8_t outputsFullyOn = (curLevel * output->outputsInChannel) >> 10;
//	uint8_t outputsFullyOn = curLevel * output->outputsInChannel / 800;

	for(int i = 0; i < outputsFullyOn; i++) {
		Tlc.set(output->firstOutput + i, params.maxBrightness);
	}

	if(output->outputsInChannel != outputsFullyOn) {
		uint16_t lastOutput = map(curLevel - (outputsFullyOn * 1024 / output->outputsInChannel), 0, 1024 / output->outputsInChannel, 0, params.maxBrightness);
		Tlc.set(output->firstOutput + outputsFullyOn, cie(lastOutput));			
	}

}


RandomBlinkEffect::RandomBlinkEffect() {
	id = 2;
	params.decay = 255;
	params.triggerLevel = 200;
	params.triggerHoldOff = 0;
	for(uint8_t i = 0; i < 16; i++) {
		levels[i] = 0;
		flags[i] = 0;
	}
}

void RandomBlinkEffect::run(uint16_t newLevel, Output *output) {
	newLevel = (long) newLevel * params.gain >> 10;

	if(newLevel > params.triggerLevel && isNotTriggerHoldOff()) {
		levels[random(output->outputsInChannel)] = newLevel;
		lastBlink = millis();
	} else {
		for(uint8_t i = 0; i < output->outputsInChannel; i++) {
			if(levels[i] >= params.decay) {
				levels[i] -= params.decay;
			} else {
				levels[i] = 0;
			}
		}
	}

	for(uint8_t i = 0; i < output->outputsInChannel; i++) {
		// Serial.println(levels[i]);
		Tlc.set(output->firstOutput + i, cie(map(levels[i], 0, 1024, 0, params.maxBrightness)));			
	}
}

ChaseEffect::ChaseEffect() {
	id = 3;
	params.decay = 512;
	params.triggerLevel = 200;
	params.triggerHoldOff = 0;
	for(uint8_t i = 0; i < 16; i++) {
		levels[i] = 0;
		flags[i] = 0;
	}
}

void ChaseEffect::run(uint16_t newLevel, Output *output) {
	newLevel = (long) newLevel * params.gain >> 10;
	
	if(newLevel > params.triggerLevel && isNotTriggerHoldOff()) {
//		if(1 != flags[0] && 1 != flags[1]) {
			levels[0] = newLevel;
			flags[0] = 1;
			lastBlink = millis();
//		}
	}

	for(uint8_t i = 0; i < output->outputsInChannel; i++) {
		if(levels[i] >= params.decay) {
			if(flags[i]) {
				levels[i] -= params.decay;
				if(i + 1 < output->outputsInChannel) {
					levels[i+1] += params.decay;
					if(levels[i+1] > 1023) {
						levels[i+1] = 1023;
					}
				}					
			}
		} else {
			if(flags[i]) {
				if(i + 1 < output->outputsInChannel) {
					levels[i+1] += levels[i];
					flags[i+1] = 1;
				}					
			}
			levels[i] = 0;
			flags[i] = 0;
		}
	}

	for(uint8_t i = 0; i < output->outputsInChannel; i++) {
		// Serial.println(levels[i]);
		if(levels[i] > 1024 || levels[i] < 0) {
			levels[1] = 1024;
		}
		Tlc.set(output->firstOutput + i, cie(map(levels[i], 0, 1024, 0, params.maxBrightness)));			
	}
}

AntiFlashEffect::AntiFlashEffect() {

}

void AntiFlashEffect::run(uint16_t newLevel, Output *output) {
	newLevel = (long) newLevel * params.gain >> 10;

	if((newLevel > params.triggerLevel) && isNotTriggerHoldOff()) {
		curLevel = newLevel;
	} else {
		if(curLevel >= params.decay) {
			curLevel -= params.decay;
		} else {
			curLevel = 0;
		}
	}
	// Serial.print(" curLevel:");
	// Serial.println(curLevel);
	// Serial.print(" newLevel:");
	// Serial.println(newLevel);

	for(uint8_t i = 0; i < output->outputsInChannel; i++) {
		Tlc.set(output->firstOutput + i, cie(params.maxBrightness - map(curLevel, 0, 1024, 0, params.maxBrightness)));			
	}	
}
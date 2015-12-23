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
	return (millis() - lastBlink) > params.asStruct.triggerHoldOff;
}

/*
bugnu flash
	params.asStruct.decay = 200;
	params.asStruct.triggerLevel = 300;

	if(isFinished() && newLevel > params.asStruct.triggerLevel) {
		curLevel = newLevel;
	} else {
		if(curLevel >= params.asStruct.decay) {
			curLevel -= params.asStruct.decay;
		} else {
			curLevel = 0;
		}
	}

*/


/*

gitaros flash

	params.asStruct.decay = 255;
	params.asStruct.triggerLevel = 10;

	if(newLevel > params.asStruct.triggerLevel) {
		curLevel = newLevel;
	} else {
		if(curLevel >= params.asStruct.decay) {
			curLevel -= params.asStruct.decay;
		} else {
			curLevel = 0;
		}
	}

*/


/*
gitara tankiai
	params.asStruct.decay = 600;
	params.asStruct.triggerLevel = 400;

	if(isFinished() && newLevel > params.asStruct.triggerLevel) {
		curLevel = newLevel;
	} else {
		if(curLevel >= params.asStruct.decay) {
			curLevel -= params.asStruct.decay;
		} else {
			curLevel = 0;
		}
	}

*/

FlashEffect::FlashEffect(){
	id = 0;
	params.asStruct.decay = 600;
	params.asStruct.triggerLevel = 400;
}

void FlashEffect::run(uint16_t newLevel, Output *output) {
	if((newLevel > params.asStruct.triggerLevel) && isNotTriggerHoldOff()) {
		curLevel = newLevel;
	} else {
		if(curLevel >= params.asStruct.decay) {
			curLevel -= params.asStruct.decay;
		} else {
			curLevel = 0;
		}
	}
	// Serial.print(" curLevel:");
	// Serial.println(curLevel);
	// Serial.print(" newLevel:");
	// Serial.println(newLevel);

	for(uint8_t i = 0; i < output->outputsInChannel; i++) {
		Tlc.set(output->firstOutput + i, cie(map(curLevel, 0, 800, 0, 4095)));			
	}
}


VUMeterEffect::VUMeterEffect() {
	id = 1;
	params.asStruct.decay = 30;
	params.asStruct.triggerLevel = 400;
	params.asStruct.triggerHoldOff = 0;
}

void VUMeterEffect::run(uint16_t newLevel, Output *output) {
	if(newLevel >= curLevel && isNotTriggerHoldOff()) {
		curLevel = newLevel;
		lastBlink = millis();
	} else {
		if(curLevel >= params.asStruct.decay) {
			curLevel -= params.asStruct.decay;
		} else {
			curLevel = 0;
		}
	}


	//curLevel / (1024 / outputs)
//	uint8_t outputsFullyOn = (curLevel * output->outputsInChannel) >> 10;
	uint8_t outputsFullyOn = curLevel * output->outputsInChannel / 800;
	// Serial.print("output->firstOutput");
	// Serial.print(output->firstOutput);
	// Serial.print(" curLevel:");
	// Serial.print(curLevel);
	// Serial.print(" newLevel:");
	// Serial.println(newLevel);
	// Serial.print(" outputsFullyOn: ");
	// Serial.println(outputsFullyOn);


	for(int i = 0; i < outputsFullyOn; i++) {
		Tlc.set(output->firstOutput + i, 4095);
	}

	if(output->outputsInChannel != outputsFullyOn) {
		uint16_t lastOutput = map(curLevel - (outputsFullyOn * 800 / output->outputsInChannel), 0, 800 / output->outputsInChannel, 0, 4095);
		Tlc.set(output->firstOutput + outputsFullyOn, cie(lastOutput));			
	}

}


/*
sax random blink
	params.asStruct.decay = 255;
	params.asStruct.triggerLevel = 500;

	if(newLevel > params.asStruct.triggerLevel && millis() - lastBlink > 100) {
		levels[random(output->outputsInChannel)] = newLevel;
		lastBlink = millis();
	} else {
		for(uint8_t i = 0; i < output->outputsInChannel; i++) {
			if(levels[i] >= params.asStruct.decay) {
				levels[i] -= params.asStruct.decay;
			} else {
				levels[i] = 0;
			}
		}
	}

*/

RandomBlinkEffect::RandomBlinkEffect() {
	id = 2;
	params.asStruct.decay = 255;
	params.asStruct.triggerLevel = 200;
	params.asStruct.triggerHoldOff = 0;
}

void RandomBlinkEffect::run(uint16_t newLevel, Output *output) {
	if(newLevel > params.asStruct.triggerLevel && isNotTriggerHoldOff()) {
		levels[random(output->outputsInChannel)] = newLevel;
		lastBlink = millis();
	} else {
		for(uint8_t i = 0; i < output->outputsInChannel; i++) {
			if(levels[i] >= params.asStruct.decay) {
				levels[i] -= params.asStruct.decay;
			} else {
				levels[i] = 0;
			}
		}
	}

	for(uint8_t i = 0; i < output->outputsInChannel; i++) {
		// Serial.println(levels[i]);
		Tlc.set(output->firstOutput + i, cie(map(levels[i], 0, 800, 0, 4095)));			
	}
}

ChaseEffect::ChaseEffect() {
	id = 3;
	params.asStruct.decay = 512;
	params.asStruct.triggerLevel = 200;
	params.asStruct.triggerHoldOff = 0;
	for(uint8_t i = 0; i < 6; i++) {
		levels[i] = 0;
		flags[i] = 0;
	}
}

void ChaseEffect::run(uint16_t newLevel, Output *output) {
	if(newLevel > params.asStruct.triggerLevel && isNotTriggerHoldOff()) {
		levels[0] = newLevel;
		flags[0] = 1;
		lastBlink = millis();
	} else {
		for(uint8_t i = 0; i < output->outputsInChannel; i++) {

			if(levels[i] >= params.asStruct.decay) {
				if(flags[i]) {
					levels[i] -= params.asStruct.decay;
					if(i + 1 < output->outputsInChannel) {
						levels[i+1] += params.asStruct.decay;
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
	}

	for(uint8_t i = 0; i < output->outputsInChannel; i++) {
		// Serial.println(levels[i]);
		Tlc.set(output->firstOutput + i, cie(map(levels[i], 0, 800, 0, 4095)));			
	}
}


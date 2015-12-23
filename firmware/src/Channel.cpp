#include <Arduino.h>
#include "Channel.h"

Channel::Channel(uint8_t _inputId, Output *_output):
	inputId(_inputId),
	output(_output) {
} 


void Channel::read(Inputs *inputs) {
	level = inputs->getVpp(inputId);
}

void Channel::setEffect(Effect *newEffect) {
	delete effect;
	effect = newEffect;
}

void Channel::runEffect() {
	if(NULL != effect) {
		// Serial.print("Level: ");
		// Serial.println(level);
		effect->run(level, output);
	} else {
		// Serial.println("No effect");
	}
}


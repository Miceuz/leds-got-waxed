#include <Arduino.h>
#include "Channel.h"
#include "Inputs.h"

Channel::Channel(uint8_t _inputId, uint8_t _firstOutput):
	inputId(_inputId),
	firstOutput(_firstOutput),
	outputsInChannel(5)
{
} 

Channel::Channel(uint8_t _inputId, uint8_t _firstOutput, uint8_t _outputsInChannel):
	inputId(_inputId),
	firstOutput(_firstOutput),
	outputsInChannel(_outputsInChannel)
{
	effect = new VUMeterEffect();
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
		Serial.print("Level: ");
		Serial.println(level);
		effect->run(level, firstOutput, outputsInChannel);
	} else {
		Serial.println("No effect");
	}

}


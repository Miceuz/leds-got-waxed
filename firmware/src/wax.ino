#include <SPI.h>
#include "DipConfig.h"
#include "Inputs.h"
#include "Channel.h"

#define PIN_DIP_LATCH 8

DipConfig dipConfig(PIN_DIP_LATCH);

PulseInputs inputs;

Channel channels[] = {Channel(0, 0), Channel(1, 5), Channel(1, 10, 6)};

void setup() {
	Tlc.init();
	Serial.begin(9600);
	SPI.begin();
	channels[0].setEffect(new FlashEffect());

}

void loop() {
	inputs.reset();
	for(int i = 0; i < 50; i++) {
		inputs.read();
	}

	for(int i = 0; i < MAX_INPUTS; i++) {
		channels[i].read(&inputs);
	}

	channels[0].runEffect();

//	effect.run(inputs.getVpp(0));
}
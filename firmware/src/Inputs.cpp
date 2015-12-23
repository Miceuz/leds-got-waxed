#include "Inputs.h"
#include <Arduino.h>

// Inputs::Inputs() {

// }

void Inputs::reset() {
	for(int i = 0; i < MAX_INPUTS; i++) {
		vol[i] = 0;
		maxVol[i] = 0;
		minVol[i] = 1023;
	}

}

void Inputs::readSamples(uint16_t samples) {
	// Serial.println("reading samples");
	for(uint16_t sample = 0; sample < samples; sample++) {
		readSample();
	}
}

inline void Inputs::readSample() {
	for(int i = 0; i < MAX_INPUTS; i++) {
		vol[i] = analogRead(pins[i]);
		// Serial.println(vol[i]);
		
		if(vol[i] > maxVol[i]) {
			maxVol[i] = vol[i];
		}

		if(vol[i] < minVol[i]) {
			minVol[i] = vol[i];
		}
	}

}

uint16_t Inputs::getVpp(uint8_t channel) {
	if(maxVol[channel] <= minVol[channel]) {
		return 0;
	}

	return maxVol[channel] - minVol[channel];
}

uint16_t PulseInputs::getVpp(uint8_t channel) {
	if(millis() - lastPulse[channel] > pulseTimeout || 0 == lastPulse[channel]) {
		lastPulse[channel] = millis();
		return 800;
	}
	return 0;
}

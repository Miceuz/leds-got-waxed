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

void Inputs::read(){
	for(int i = 0; i < MAX_INPUTS; i++) {
		vol[i] = analogRead(pins[i]);
		
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
	if(0 == channel && (millis() - lastPulse > pulseTimeout || 0 == lastPulse)) {
		lastPulse = millis();
		return 1023;
	}
	return 0;
}

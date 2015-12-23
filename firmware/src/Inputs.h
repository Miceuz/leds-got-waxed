#ifndef _INPUTS_H
#define _INPUTS_H

#include <Arduino.h>
#include <inttypes.h>
#define MAX_INPUTS 3

class Inputs {
public:
//	Inputs();
	virtual void readSamples(uint16_t samples);
	virtual uint16_t getVpp(uint8_t channel);
	virtual void reset();
private:
	inline void readSample();
	uint8_t pins[MAX_INPUTS] = {A0, A1, A2};
	uint16_t vol[MAX_INPUTS];  
	uint16_t maxVol[MAX_INPUTS] = {0, 0, 0};
	uint16_t minVol[MAX_INPUTS] = {1024, 1024, 1024};

};

class DummyInputs : public Inputs {
public:
	// virtual void readSamples(uint16_t samples) {};
	virtual void reset() {};
	uint16_t getVpp(uint8_t channel);
};

class PulseInputs : public DummyInputs {
public:
	PulseInputs(uint16_t _pulseTimeout = 1000) : pulseTimeout(_pulseTimeout){};
	uint16_t getVpp(uint8_t channel);
private:
	uint32_t lastPulse[3];
	uint16_t pulseTimeout = 1000;
};

// class RampsInputs : public DummyInputs {
	
// };

#endif
#ifndef _INPUTS_H
#define _INPUTS_H

#include <Arduino.h>
#include <inttypes.h>
#define MAX_INPUTS 3

class Inputs {
public:
//	Inputs();
	virtual void read();
	virtual uint16_t getVpp(uint8_t channel);
	virtual void reset();
private:
	uint8_t pins[MAX_INPUTS];
	uint16_t vol[MAX_INPUTS];  
	uint16_t maxVol[MAX_INPUTS] = {0, 0, 0};
	uint16_t minVol[MAX_INPUTS] = {1024, 1024, 1024};

};

class DummyInputs : public Inputs {
public:
	virtual void read() {};
	virtual void reset() {};
	virtual uint16_t getVpp(uint8_t channel);
};

class PulseInputs : public Inputs {
public:
	virtual void read() {};
	virtual uint16_t getVpp(uint8_t channel);
private:
	uint32_t lastPulse;
	uint16_t pulseTimeout = 3000;
};

// class RampsInputs : public DummyInputs {
	
// };

#endif
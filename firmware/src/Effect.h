#ifndef _EFFECT_H
#define _EFFECT_H

#include "Tlc5940.h"

class Effect{
public:
	Effect();
	virtual ~Effect();
	virtual uint8_t isFinished();
	virtual uint8_t canInterrupt();
	virtual void run(uint16_t newLevel, uint8_t firstOutput, uint8_t outputs);
protected:
	uint16_t curLevel = 0;
	uint16_t decay = 10;	
};

class VUMeterEffect : public Effect {
public:
	VUMeterEffect();
	uint8_t isFinished();
	void run(uint16_t newLevel, uint8_t firstOutput, uint8_t outputs);
};

class FlashEffect : public Effect {
public:
	FlashEffect();
	uint8_t isFinished();
	uint8_t canInterrupt();
	void run(uint16_t newLevel, uint8_t firstOutput, uint8_t outputs);
private:
	uint16_t triggerLevel = 100;
};


#endif
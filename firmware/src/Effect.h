#ifndef _EFFECT_H
#define _EFFECT_H

#include "Tlc5940.h"
#include "Output.h"

class EffectParams {
public:
	uint16_t attack = 0;
	uint16_t decay = 40;
	uint16_t triggerLevel = 100;	
	uint16_t triggerHoldOff = 0;
	uint16_t maxBrightness = 4095;	
	uint16_t gain = 1300;	
};

class Effect{
public:
	Effect();
	virtual ~Effect();
	virtual void run(uint16_t newLevel, Output *output);

	uint8_t id;
	EffectParams params;

protected:
	int16_t levels[16];		
	uint8_t flags[16];		

	uint16_t curLevel = 0;
	uint32_t lastBlink;
	uint8_t isNotTriggerHoldOff();
};

class VUMeterEffect : public Effect {
public:
	VUMeterEffect();
	void run(uint16_t newLevel, Output *output);
};

class FlashEffect : public Effect {
public:
	FlashEffect();
	void run(uint16_t newLevel, Output *output);
};

class RandomBlinkEffect : public Effect {
public:
	RandomBlinkEffect();
	void run(uint16_t newLevel, Output *output);
};

class ChaseEffect : public Effect {
public:
	ChaseEffect();
	void run(uint16_t newLevel, Output *output);
};

class AntiFlashEffect : public Effect {
public:
	AntiFlashEffect();
	void run(uint16_t newLevel, Output *output);
};

#endif
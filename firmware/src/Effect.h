#ifndef _EFFECT_H
#define _EFFECT_H

#include "Tlc5940.h"
#include "Output.h"

class EffectParams {
public:
	EffectParams(){};
	union {
		struct {
			uint16_t attack = 0;
			uint16_t decay = 40;
			uint16_t triggerLevel = 100;	
			uint16_t triggerHoldOff = 0;	
		} asStruct;
		uint16_t asArray[3];
	};
};

class Effect{
public:
	Effect();
	virtual ~Effect();
	virtual void run(uint16_t newLevel, Output *output);

	uint8_t id;
	EffectParams params;

protected:
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
private:
	uint16_t levels[6];	
};

class ChaseEffect : public Effect {
public:
	ChaseEffect();
	void run(uint16_t newLevel, Output *output);
private:
	uint16_t levels[6];		
	uint8_t flags[6];		
};
#endif
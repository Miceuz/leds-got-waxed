#include "DipConfig.h"
#include "Inputs.h"
#include "Channel.h"
#include "Output.h"
#include <Conceptinetics.h>

#define PIN_DIP_CLK 4
#define PIN_DIP_DATA 6
#define PIN_DIP_LATCH 5
DipConfig dipConfig(PIN_DIP_LATCH, PIN_DIP_CLK, PIN_DIP_DATA);

#define DMX_CHANNELS_PER_CHANNEL 7
#define DMX_SLAVE_CHANNELS DMX_CHANNELS_PER_CHANNEL * MAX_INPUTS 
void onDMXFrameReceived (void);
DMX_Slave dmx_slave ( DMX_SLAVE_CHANNELS, 2);
unsigned long       lastFrameReceivedTime;


Channel channels[] = {
	Channel(0, new Output(0, 5)), 
	Channel(1, new Output(5, 5)), 
	Channel(2, new Output(10, 6))};

Inputs *inputs;

uint32_t lastOutputFrameSent = 0;
uint8_t takeOverInProgress = 0;
uint8_t takeOverChannel = 0;

Effect* toEffect(uint8_t effectId);

uint8_t dmxGetEffectId(uint8_t channel);
uint16_t dmxGetDecay(uint8_t channel);
uint16_t dmxGetTriggerLevel(uint8_t channel);
uint16_t dmxGetTriggerHoldOff(uint8_t channel);
uint8_t dmxIsDataAvaialble();
uint16_t dmxGetBrightness(uint8_t channel);
uint16_t dmxGetGain(uint8_t channel);
uint8_t dmxIsTakeover(uint8_t channel);
uint8_t dmxIsBlackout();

void blackout();
void takeOverAllOutputs(uint8_t takeoverChannel);
void releaseTakeOver();

void setup() {
	ADCSRA = _BV(ADEN) | _BV(ADPS1) | _BV(ADPS2);
	randomSeed(analogRead(0));

	dmx_slave.enable ();  
	dmx_slave.setStartAddress (1);
	dmx_slave.onReceiveComplete (onDMXFrameReceived);

	Tlc.init();
	// Serial.begin(9600);
	channels[0].setEffect(new ChaseEffect());
	channels[1].setEffect(new ChaseEffect());
	channels[2].setEffect(new ChaseEffect());

	if(dipConfig.isTestMode()) {
		inputs = new PulseInputs();
	} else {
		inputs = new Inputs();
	}
}


inline static uint8_t isFrameTimeout() {
	return millis() - lastOutputFrameSent > 30;
}


void loop() {
	//TODO paoptimizuoti šito laiką
	//Apatiniame rėžyje sempluotume 20Hz, jei
	//nuskaitytume bent 300 semplų per freimą.
	inputs->readSamples(100);

	if(isFrameTimeout()) {
		Tlc.clear();

		if(dmxIsDataAvaialble() && dmxIsBlackout()) {
			blackout();
		} else {
			for(int i = 0; i < MAX_INPUTS; i++) {
				channels[i].read(inputs);
				if(0 != channels[i].output->outputsInChannel) {
					channels[i].runEffect();
				}
			}
			Tlc.update();  
		}

		lastOutputFrameSent = millis();

		if(dmxIsDataAvaialble()) {
			for(uint8_t i = 0; i < MAX_INPUTS; i++) {
				if(channels[i].effect->id != dmxGetEffectId(i)) {
					channels[i].setEffect(toEffect(dmxGetEffectId(i)));
				}
				channels[i].effect->params.asStruct.decay = dmxGetDecay(i);
				channels[i].effect->params.asStruct.triggerLevel = dmxGetTriggerLevel(i);
				channels[i].effect->params.asStruct.triggerHoldOff = dmxGetTriggerHoldOff(i);
				channels[i].effect->params.asStruct.maxBrightness = dmxGetBrightness(i);
				channels[i].effect->params.asStruct.gain = dmxGetGain(i);
				
				if(!takeOverInProgress) {
					if(dmxIsTakeover(i)) {
						takeOverAllOutputs(i);
					}
				} else {
					if(i == takeOverChannel && !dmxIsTakeover(i)) {
						releaseTakeOver();
					}
				}
			}
		}
		inputs->reset();
	}
}

uint8_t dmxIsDataAvaialble() {
	return lastFrameReceivedTime > 0 && millis() - lastFrameReceivedTime < 1000;
}

uint8_t dmxGetEffectId(uint8_t channel) {
	return dmx_slave.getChannelValue (DMX_CHANNELS_PER_CHANNEL * channel + 1) / 52;
}

uint16_t dmxGetDecay(uint8_t channel) {
	return map(dmx_slave.getChannelValue(DMX_CHANNELS_PER_CHANNEL * channel + 2), 0, 255, 1, 512);
}

uint16_t dmxGetTriggerLevel(uint8_t channel) {
	return map(dmx_slave.getChannelValue(DMX_CHANNELS_PER_CHANNEL * channel + 3), 0, 255, 1, 1024);
}

uint16_t dmxGetTriggerHoldOff(uint8_t channel) {
	return map(dmx_slave.getChannelValue(DMX_CHANNELS_PER_CHANNEL * channel + 4), 0, 255, 0, 1000);
}

uint16_t dmxGetBrightness(uint8_t channel) {
	return map(dmx_slave.getChannelValue(DMX_CHANNELS_PER_CHANNEL * channel + 5), 0, 255, 0, 4095);
}

uint16_t dmxGetGain(uint8_t channel) {
	return map(dmx_slave.getChannelValue(DMX_CHANNELS_PER_CHANNEL * channel + 6), 0, 255, 256, 2048);
}

uint8_t dmxIsTakeover(uint8_t channel) {
	return dmx_slave.getChannelValue(DMX_CHANNELS_PER_CHANNEL * channel + 7) > 0;
}

uint8_t dmxIsBlackout() {
	for(int i = 0; i < DMX_SLAVE_CHANNELS; i++) {
		if(0 != dmx_slave.getChannelValue(i+1)) {
			return 0;
		}
	}
	return 1;
}

Effect* toEffect(uint8_t effectId) {
	switch (effectId) {
		case 0: return new FlashEffect();
		case 1: return new VUMeterEffect();
		case 2: return new RandomBlinkEffect();
		case 3: return new ChaseEffect();
		case 4: return new AntiFlashEffect();
	} 
	return new FlashEffect();
}

void onDMXFrameReceived (void) {
	lastFrameReceivedTime = millis ();
}

void blackout() {
	Tlc.clear();
	Tlc.update();
}

void takeOverAllOutputs(uint8_t takeoverChannel) {
	takeOverInProgress = 1;
	takeOverChannel = takeoverChannel;

	for(uint8_t i = 0; i < MAX_INPUTS; i++) {
		if(i == takeoverChannel) {
			channels[i].output->outputsInChannel = 16;
			channels[i].output->firstOutput = 0;
		} else {
			channels[i].output->outputsInChannel = 0;
		}
	}
}

void releaseTakeOver() {
	takeOverInProgress = 0;

	channels[0].output->outputsInChannel = 5;
	channels[0].output->firstOutput = 0;

	channels[1].output->outputsInChannel = 5;
	channels[1].output->firstOutput = 5;

	channels[2].output->outputsInChannel = 6;
	channels[2].output->firstOutput = 10;
}

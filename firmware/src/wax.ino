 #include <SPI.h>
#include "DipConfig.h"
#include "Inputs.h"
#include "Channel.h"
#include "Output.h"
#include <Conceptinetics.h>

#define DMX_CHANNELS_PER_CHANNEL 7

#define DMX_SLAVE_CHANNELS DMX_CHANNELS_PER_CHANNEL * MAX_INPUTS 
#define PIN_DIP_LATCH 8

#include "memdebug.h"

DMX_Slave dmx_slave ( DMX_SLAVE_CHANNELS, 2);
unsigned long       lastFrameReceivedTime;
void onDMXFrameReceived (void);

DipConfig dipConfig(PIN_DIP_LATCH);

Inputs *inputs;
Inputs adcInputs;
PulseInputs pulseDummyInputs;

Effect* toEffect(uint8_t effectId);
uint8_t dmxGetEffectId(uint8_t channel);
uint16_t dmxGetDecay(uint8_t channel);
uint16_t dmxGetTriggerLevel(uint8_t channel);
uint16_t dmxGetTriggerHoldOff(uint8_t channel);
uint8_t dmxIsDataAvaialble();
uint16_t dmxGetBrightness(uint8_t channel);
uint8_t dmxIsBlackout();
void blackout();

Channel channels[] = {Channel(0, new Output(0,5)), Channel(1, new Output(5, 5)), Channel(2, new Output(10, 6))};
//Channel channels[] =   {Channel(0, new Output(0,15)), Channel(0, new Output(15, 1)), Channel(5, new Output(15,1))};
void setup() {
	dmx_slave.enable ();  
	dmx_slave.setStartAddress (1);
	dmx_slave.onReceiveComplete (onDMXFrameReceived);

	ADCSRA = _BV(ADEN) | _BV(ADPS1) | _BV(ADPS2);
	Tlc.init();
	SPI.begin();
	// Serial.begin(9600);
	channels[0].setEffect(new ChaseEffect());
	channels[1].setEffect(new ChaseEffect());
	channels[2].setEffect(new ChaseEffect());

//	Serial.println(dipConfig.get());
	// if(dipConfig.isTestMode()) {
	// 	Serial.println("test mode");
	// 	inputs = new PulseInputs();
	// } else {
	// 	inputs = new Inputs();
	// }

    inputs = new Inputs();
//	inputs = new PulseInputs();
	randomSeed(analogRead(0));
}

int freeRam();

uint32_t lastOutputFrameSent = 0;
uint8_t b=0;
uint16_t frames;

inline static uint8_t isFrameTimeout() {
	return millis() - lastOutputFrameSent > 30;
}

uint32_t start;
extern int __heap_start, *__brkval;

void loop() {
	//TODO paoptimizuoti šito laiką
	//Apatiniame rėžyje sempluotume 20Hz, jei
	//nuskaitytume bent 300 semplų per freimą.
	start = millis();
	inputs->readSamples(100);

	if(isFrameTimeout()) {
		Tlc.clear();

		if(dmxIsDataAvaialble() && dmxIsBlackout()) {
			blackout();
		} else {
			for(int i = 0; i < MAX_INPUTS; i++) {
				channels[i].read(inputs);
				channels[i].runEffect();
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
			}
		}
		inputs->reset();
	}
}

uint8_t dmxIsDataAvaialble() {
	return lastFrameReceivedTime > 0 && millis() - lastFrameReceivedTime < 1000;
}


uint8_t dmxGetEffectId(uint8_t channel) {
	return dmx_slave.getChannelValue (DMX_CHANNELS_PER_CHANNEL * channel + 1) / 64;
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

uint8_t dmxIsBlackout() {
	for(int i = 0; i < DMX_SLAVE_CHANNELS; i++) {
		if(0 != dmx_slave.getChannelValue(i+1)) {
			return 0;
		}
	}
	return 1;
}

uint16_t dmxGetBrightness(uint8_t channel) {
	return map(dmx_slave.getChannelValue(DMX_CHANNELS_PER_CHANNEL * channel + 5), 0, 255, 0, 4095);
}

Effect* toEffect(uint8_t effectId) {
	switch (effectId) {
		case 0: return new FlashEffect();
		case 1: return new VUMeterEffect();
		case 2: return new RandomBlinkEffect();
		case 3: return new ChaseEffect();
	} 
	return new FlashEffect();
}

void onDMXFrameReceived (void) {
	lastFrameReceivedTime = millis ();
}

void blackout() {
	// for(int i = 0; i < 16; i++) {
	// 	Tlc.set(i, 0);
	// }
	Tlc.clear();
	Tlc.update();
}

// int freeRam () {
//   extern int __heap_start, *__brkval; 
//   int v; 
//   return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
// }

//ChaseEffect
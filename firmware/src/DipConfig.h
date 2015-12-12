#ifndef _DIP_CONFIG_H
#define _DIP_CONFIG_H
#include <inttypes.h>

class DipConfig
{
public:
	DipConfig(uint8_t _latchPin);
	uint16_t get();
private:
	uint8_t latchPin;	
};

#endif
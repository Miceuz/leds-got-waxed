#ifndef _OUTPUT_H
#define _OUTPUT_H
#include <inttypes.h>

class Output {
public:
	Output(uint8_t _firstOutput, uint8_t _outputsInChannel = 5):	
		firstOutput(_firstOutput),
		outputsInChannel(_outputsInChannel)
	{};
	uint8_t firstOutput;
	uint8_t outputsInChannel;

};
#endif
#include <inttypes.h>
#include "Effect.h"
#include "Inputs.h"
class Channel {
public:
	Channel(uint8_t _inputId, uint8_t _firstOutput);
	Channel(uint8_t _inputId, uint8_t _firstOutput, uint8_t _outputsInChannel);
	void read(Inputs *inputs);
	void setEffect(Effect *newEffect);
	void runEffect();
private:
	uint8_t inputId;
	uint8_t firstOutput;
	uint8_t outputsInChannel;
	uint16_t level;
	Effect *effect;
};
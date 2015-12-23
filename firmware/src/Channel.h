#include <inttypes.h>
#include "Effect.h"
#include "Inputs.h"
#include "Output.h"

class Channel {
public:
	Channel(uint8_t _inputId, Output *_output);
	void read(Inputs *inputs);
	void setEffect(Effect *newEffect);
	void runEffect();
	inline Effect *getEffect() {return effect;};
	uint8_t inputId;
	Effect *effect;
private:
	uint16_t level;
	Output *output;
};
#ifndef NeedySlave_h
#define NeedySlave_h

#include <Slave.h>

#define STATE_SLEEPING 1
#define STATE_ACTIVE 2

class NeedySlave : public Slave {

	uint16_t activationThresholdInSecs[3] = { 15 * 60, 10 * 60, 5 * 60 };
	uint16_t activationTimeInSecs = 0;

protected:

	void updateTimeRemaining(uint16_t secs) override;
	virtual void activate();
	void arm() override;
public:

	NeedySlave(uint8_t i2cAddress, uint8_t raiseInterruptPin);
};

#endif
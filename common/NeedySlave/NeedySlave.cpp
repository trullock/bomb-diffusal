#include "./NeedySlave.h"

NeedySlave::NeedySlave(byte i2caddress, uint8_t raiseInterruptPin) : Slave(i2caddress, raiseInterruptPin)
{
}

void NeedySlave::arm()
{
	Slave::arm();
	this->state = STATE_SLEEPING;
}

void NeedySlave::updateTimeRemaining(uint16_t secs)
{
	uint16_t activationThresholdInSecs = this->activationThresholdInSecs[this->difficulty];
	if(secs > activationThresholdInSecs)
	{
		// recalculate this every time to support the time remaining not decreasing sequentially (e.g. time pentaly strikes)
		randomSeed(micros());
		this->activationTimeInSecs = random(activationThresholdInSecs, secs);
	}
	else
	{
		this->activationTimeInSecs = secs;
	}
	
	this->timeRemainingInSecs = secs;

	if(this->state == STATE_SLEEPING && secs <= this->activationTimeInSecs)
	{
		this->state = STATE_ACTIVE;
		this->activate();
		return;
	}
}
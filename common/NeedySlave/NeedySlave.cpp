#include "./NeedySlave.h"

NeedySlave::NeedySlave(int i2caddress) : Slave(i2caddress)
{

}


void NeedySlave::arm()
{
	Slave::arm();
	this->state = STATE_SLEEPING;
}

void NeedySlave::updateTimeRemaining(unsigned int secs)
{
	byte activationThresholdInSecs = this->activationThresholdInSecs[this->difficulty];
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
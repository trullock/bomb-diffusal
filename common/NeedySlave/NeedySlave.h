#include <Slave.h>


#define STATE_SLEEPING 1
#define STATE_ACTIVE 2

class NeedySlave : public Slave {

	unsigned int activationThresholdInSecs[3] = { 15 * 60, 10 * 60, 5 * 60 };
	unsigned int activationTimeInSecs = 0;

protected:

	void updateTimeRemaining(unsigned int secs) override;
	virtual void activate();
	void arm() override;
public:

	NeedySlave(int i2cAddress);
};
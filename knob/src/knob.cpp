#include <slave.h>
#include "../lib/button.h"

#define STATE_SLEEPING 1
#define STATE_ACTIVE 2

class Knob : public Slave {

	unsigned long nextMillis = 0;

	byte knobPosition = 0;

	Button* btn1;
	Button* btn2;
	Button* btn3;
	Button* btn4;

	byte activationTimeInM[3] = { 15, 10, 5 };

	void setDifficulty(byte diff) override
	{
		Slave::setDifficulty(diff);
	}

	void arm()
	{
		Slave::arm();

		this->state = STATE_SLEEPING;
	}

	void strike() override {
		Slave::strike();

		this->nextMillis = millis() + STRIKE_DURATION_MS;
	}

	void explode() override
	{
		Slave::explode();
	}

	void deactivate() override
	{
		Slave::deactivate();
	}

	void maybeWakeUp()
	{
		if (timeRemainingInMins <= activationTimeInM[this->difficulty])
		{
			this->state = STATE_ACTIVE;
			return;
		}
	}

	void handleKnob()
	{
		if(this->btn1->pressed())
			this->knobPosition = 1;
		else if(this->btn2->pressed())
			this->knobPosition = 2;
		else if(this->btn3->pressed())
			this->knobPosition = 3;
		else if(this->btn4->pressed())
			this->knobPosition = 4;
	}

	void reportStrike() override
	{
		Slave::reportStrike();
	}

	void stopStriking()
	{
		this->state = this->preStrikeState;
	}

public:
	Knob() : Slave(4)
	{
		this->btn1 = new Button(9, INPUT_PULLUP);
		this->btn2 = new Button(6, INPUT_PULLUP);
		this->btn3 = new Button(7, INPUT_PULLUP);
		this->btn4 = new Button(8, INPUT_PULLUP);
	}

	void loop()
	{
		Slave::loop();

		if(this->state == STATE_SLEEPING)
		{
			this->maybeWakeUp();
		}
		else if (this->state == STATE_ACTIVE)
		{
			this->handleKnob();
		}
		else if(this->state == STATE_STRIKING)
		{
			if(millis() >= nextMillis)
				this->stopStriking();
		}
	}
};
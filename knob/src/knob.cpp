#include <NeedySlave.h>
#include "../lib/button.h"

class Knob : public NeedySlave {

	unsigned long nextMillis = 0;

	byte knobPosition = 0;
	byte preStrikeState = 0;

	Button* btn1;
	Button* btn2;
	Button* btn3;
	Button* btn4;


	void setDifficulty(byte diff) override
	{
		NeedySlave::setDifficulty(diff);
	}

	void arm()
	{
		NeedySlave::arm();

		this->state = STATE_SLEEPING;
	}

	void strike() override {
		NeedySlave::strike();

		this->nextMillis = millis() + STRIKE_DURATION_MS;
	}

	void explode() override
	{
		NeedySlave::explode();
	}

	void deactivate() override
	{
		NeedySlave::deactivate();
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
		NeedySlave::reportStrike();
	}

	void stopStriking()
	{
		this->state = this->preStrikeState;
	}

public:
	Knob() : NeedySlave(4)
	{
		this->btn1 = new Button(9, INPUT_PULLUP);
		this->btn2 = new Button(6, INPUT_PULLUP);
		this->btn3 = new Button(7, INPUT_PULLUP);
		this->btn4 = new Button(8, INPUT_PULLUP);
	}

	void loop()
	{
		NeedySlave::loop();

		if (this->state == STATE_ACTIVE)
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
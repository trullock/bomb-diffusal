#include <NeedySlave.h>
#include <Adafruit_SSD1306.h>
#include "capacitor.h"

class Capacitors : public NeedySlave {

	Capacitor cap1;
	//Capacitor cap2;

	void arm() override
	{
		NeedySlave::arm();
		cap1.reset(this->difficulty);
		//cap2.reset(this->difficulty);
	}

	void reportStrike()
	{
		NeedySlave::reportStrike();
		cap1.reset(this->difficulty);
		//cap2.reset(this->difficulty);
	}

public:

	Capacitors() :
		NeedySlave(9, 10)
		, cap1(3, 2, 7, 8)
		//,cap2(4, 5, 8, 9)
	{
		// development hacks
		this->setDifficulty(2);
		this->arm();
		this->updateTimeRemaining(30);
	}

	void setDifficulty(byte diff)
	{
		NeedySlave::setDifficulty(diff);
		this->cap1.reset(diff);
	}

	void updateTimeRemaining(uint16_t secs)
	{
		NeedySlave::updateTimeRemaining(secs);
	}

	void activate()
	{
		Serial.println("Activating");
		this->cap1.activate();
		//this->cap2.activate();
	}

	void loop()
	{
		NeedySlave::loop();

		if(this->state != STATE_ACTIVE)
			return;

		this->cap1.loop();
		//this->cap2.loop();

		if(this->cap1.chargeLevelOOB())
		{
			this->reportStrike();
			this->cap1.reset(this->difficulty);
		}

		// if(this->cap2.chargeLevelOOB())
		// {
		// 	this->reportStrike();
		// 	this->cap2.reset();
		// }
	}
};
#include <NeedySlave.h>
#include <Encoder.h>
#include <Adafruit_SSD1306.h>
#include "capacitor.h"

class Capacitors : public NeedySlave {

	Capacitor cap1;
	Capacitor cap2;

	void arm() override
	{
		NeedySlave::arm();
		cap1.reset();
		cap2.reset();
	}

	void reportStrike()
	{
		NeedySlave::reportStrike();
		cap1.reset();
		cap2.reset();
	}

public:

	Capacitors() :
		NeedySlave(9),
		cap1(4, 5, 8, 9),
		cap2(4, 5, 8, 9)
	{
		// development hacks
		this->setDifficulty(2);
		this->arm();
		this->updateTimeRemaining(30);
	}

	void updateTimeRemaining(uint16_t secs)
	{
		NeedySlave::updateTimeRemaining(secs);
	}

	void activate()
	{
		Serial.println("Activating");
		this->cap1.activate();
		this->cap2.activate();
	}

	void loop()
	{
		NeedySlave::loop();

		if(this->state != STATE_ACTIVE)
			return;

		this->cap1.loop();
		this->cap2.loop();

		if(this->cap1.chargeLevelOOB())
		{
			this->reportStrike();
			this->cap1.reset();
		}

		if(this->cap2.chargeLevelOOB())
		{
			this->reportStrike();
			this->cap2.reset();
		}
	}
};
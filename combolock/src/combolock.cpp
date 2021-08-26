#include <slave.h>
#include <Encoder.h>
#include "../lib/button.h"
#include <LedControl.h>

#define STATE_MORSING 1

#define WordCount 5
#define Morse_LED_Pin 5

#define DIRECTION_LEFT 0
#define DIRECTION_RIGHT 1

class Combolock : public Slave {

	unsigned long nextMillis;

	/// Button details
	Button btnEnter;

	//// Knob details
	Encoder knob;
	int knobPosition = 0;
	bool correctDirection = DIRECTION_LEFT;
	byte combination[5];

	// Display
	LedControl display;

	void setDifficulty(byte diff) override
	{
		Slave::setDifficulty(diff);

		memset(this->combination, 0, 5);

		this->combination[0] = random(1, 256);
		this->combination[1] = random(1, 256);
		this->combination[2] = random(1, 256);

		if(diff > 0)
			this->combination[3] = random(1, 256);
		
		if(diff == 2)
			this->combination[4] = random(1, 256);
	}

	void arm()
	{
		Slave::arm();

		this->state = STATE_MORSING;
	}

	void strike() override {
		Slave::strike();

		this->nextMillis = millis() + STRIKE_DURATION_MS;
		this->setLed(true);

		display.setDigit(0, 0, 8, true);
		display.setDigit(0, 1, 8, true);
		display.setDigit(0, 2, 8, true);
		display.setDigit(0, 3, 8, true);
	}

	void explode() override
	{
		Slave::explode();
	}

	void deactivate() override
	{
		Slave::deactivate();

		display.setIntensity(0, 1);
		display.setChar(0, 0, '-', false);
		display.setChar(0, 1, '-', false);
		display.setChar(0, 2, '-', false);
		display.setChar(0, 3, '-', false);
	}

	void handleKnob()
	{
		int position = knob.read();
		if (position != knobPosition)
		{
			knobPosition = position;
		}
	}

	void handleButton()
	{
		if (!this->btnEnter.pressed())
			return;

		if (currentFrequency == correctFrequency)
		{
			this->deactivate();
			return;
		}

		this->reportStrike();
	}

	void updateDisplay()
	{
		int thousands = currentFrequency / 1000;
		int hundreds = (currentFrequency - thousands * 1000) / 100;
		int tens = (currentFrequency - thousands * 1000 - hundreds * 100) / 10;
		int ones = currentFrequency % 10;

		display.setDigit(0, 0, ones, false);
		display.setDigit(0, 1, tens, false);
		display.setDigit(0, 2, hundreds, false);
		display.setDigit(0, 3, thousands, true);
	}

	void setLed(bool on)
	{
		morseLedOn = on;
		digitalWrite(Morse_LED_Pin, on ? HIGH : LOW);
	}

	void stopStriking()
	{
		this->state = STATE_MORSING;
		this->setLed(false);
		this->currentMorseChar = "";
		this->currentCharIndex = -1;
		this->nextMillis = millis() + this->loopDelay[this->difficulty];
	}

public:
	Combolock() : 
		Slave(12), 
		btnEnter(8, INPUT_PULLUP), 
		knob(6, 7),
		display(12, 11, 10, 1)
	{
		pinMode(Morse_LED_Pin, OUTPUT);

		this->display.shutdown(0, false);
		this->display.setIntensity(0, 8);
		this->display.clearDisplay(0);
	}

	void loop()
	{
		Slave::loop();

		if(this->state == STATE_MORSING)
		{
			this->updateDisplay();

			this->handleButton();
			this->handleKnob();
		}
		else if(this->state == STATE_STRIKING)
		{
			if(millis() >= nextMillis)
				this->stopStriking();
		}
	}
};
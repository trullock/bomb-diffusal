#include "../lib/slave/slave.h"
#include <Encoder.h>
#include "../lib/button.h"

#define STATE_STAGE1 1
#define STATE_STAGE2 2
#define STATE_STAGE3 3
#define STATE_STAGE4 4
#define STATE_STAGE5 5
#define STATE_STAGE6 6

class Memory : public Slave {

	unsigned long nextMillis = 0;

	Button *btn1;
	Button* btn2;
	Button* btn3;
	Button* btn4;

	byte currentClue = 0;
	byte stages = 0;

	void setDifficulty(byte diff) override
	{
		Slave::setDifficulty(diff);

		switch(diff) {
			case 0:
				this->stages = 2;
			case 1:
				this->stages = 4;
			case 2:
				this->stages = 6;
		}

		randomSeed(analogRead(0));
		
		this->nextStage(STATE_STAGE1);
	}

	void nextStage(byte stage)
	{
		this->state = stage;

		this->currentClue = random(0, 4);
		Serial.print("Current clue: ");
		Serial.println(this->currentClue);
	}

	void arm()
	{
		Slave::arm();

		this->state = STATE_STAGE1;
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

	void handleButtons()
	{
		byte btnPressed = 0;

		if(this->btn1->pressed())
			btnPressed = 1;
		else if(this->btn2->pressed())
			btnPressed = 2;
		else if(this->btn3->pressed())
			btnPressed = 3;
		else if(this->btn4->pressed())
			btnPressed = 4;

		if(btnPressed == 0)
			return;

		if(this->state == 1)
			this->handleStage1(btnPressed);
		else if (this->state == 2)
			this->handleStage2(btnPressed);
		else if (this->state == 3)
			this->handleStage3(btnPressed);
		else if (this->state == 4)
			this->handleStage4(btnPressed);
		else if (this->state == 5)
			this->handleStage5(btnPressed);
		else if (this->state == 6)
			this->handleStage6(btnPressed);
	}

	void handleStage1(byte btnPressed)
	{
		switch (this->currentClue)
		{
			case 1:
				if(btnPressed == 2)
					this->nextStage(STATE_STAGE2);
				else
					reportStrike();
				break;
			case 2:
				if (btnPressed == 2)
					this->nextStage(STATE_STAGE2);
				else
					reportStrike();
				break;
			case 3:
				if (btnPressed == 3)
					this->nextStage(STATE_STAGE2);
				else
					reportStrike();
				break;
			case 4:
				if (btnPressed == 4)
					this->nextStage(STATE_STAGE2);
				else
					reportStrike();
				break;
		}
	}

	void handleStage2(byte btnPressed)
	{
		switch (this->currentClue)
		{
		case 1:
			if (btnPressed == 3)
				this->nextStage(STATE_STAGE3);
			else
				reportStrike();
			break;
		case 2:
			if (btnPressed == 2)
				this->nextStage(STATE_STAGE3);
			else
				reportStrike();
			break;
		case 3:
			if (btnPressed == 1)
				this->nextStage(STATE_STAGE3);
			else
				reportStrike();
			break;
		case 4:
			if (btnPressed == 4)
				this->nextStage(STATE_STAGE3);
			else
				reportStrike();
			break;
		}
	}

	void handleStage3(byte btnPressed)
	{
		switch (this->currentClue)
		{
		case 1:
			if (btnPressed == 2)
				this->nextStage(STATE_STAGE4);
			else
				reportStrike();
			break;
		case 2:
			if (btnPressed == 2)
				this->nextStage(STATE_STAGE4);
			else
				reportStrike();
			break;
		case 3:
			if (btnPressed == 3)
				this->nextStage(STATE_STAGE4);
			else
				reportStrike();
			break;
		case 4:
			if (btnPressed == 3)
				this->nextStage(STATE_STAGE4);
			else
				reportStrike();
			break;
		}
	}

	void handleStage4(byte btnPressed)
	{
		switch (this->currentClue)
		{
		case 1:
			if (btnPressed == 2)
				this->nextStage(STATE_STAGE5);
			else
				reportStrike();
			break;
		case 2:
			if (btnPressed == 1)
				this->nextStage(STATE_STAGE5);
			else
				reportStrike();
			break;
		case 3:
			if (btnPressed == 1)
				this->nextStage(STATE_STAGE5);
			else
				reportStrike();
			break;
		case 4:
			if (btnPressed == 4)
				this->nextStage(STATE_STAGE5);
			else
				reportStrike();
			break;
		}
	}

	void handleStage5(byte btnPressed)
	{
		switch (this->currentClue)
		{
		case 1:
			if (btnPressed == 2)
				this->nextStage(STATE_STAGE6);
			else
				reportStrike();
			break;
		case 2:
			if (btnPressed == 1)
				this->nextStage(STATE_STAGE6);
			else
				reportStrike();
			break;
		case 3:
			if (btnPressed == 1)
				this->nextStage(STATE_STAGE6);
			else
				reportStrike();
			break;
		case 4:
			if (btnPressed == 4)
				this->nextStage(STATE_STAGE6);
			else
				reportStrike();
			break;
		}
	}

	void updateDisplay()
	{
	}

	void stopStriking()
	{
		this->state = STATE_MORSING;
	}

public:
	Memory() : Slave(4)
	{
		this->btn1 = new Button(9, INPUT_PULLUP);
		this->btn2 = new Button(6, INPUT_PULLUP);
		this->btn3 = new Button(7, INPUT_PULLUP);
		this->btn4 = new Button(8, INPUT_PULLUP);
	}

	void loop()
	{
		Slave::loop();

		if (this->state == STATE_STAGE1 || this->state == STATE_STAGE2 || this->state == STATE_STAGE3 || this->state == STATE_STAGE4 || this->state == STATE_STAGE5 || this->state == STATE_STAGE6)
		{
			this->handleButtons();
			this->updateDisplay();
		}
		else if(this->state == STATE_STRIKING)
		{
			if(millis() >= nextMillis)
				this->stopStriking();
		}
	}
};
#include <slave.h>
#include <Encoder.h>
#include "../lib/button.h"
#include <Adafruit_SSD1306.h>

#define STATE_STAGE1 1
#define STATE_STAGE2 2
#define STATE_STAGE3 3
#define STATE_STAGE4 4
#define STATE_STAGE5 5
#define STATE_STAGE6 6

class Memory : public Slave {

	unsigned long nextMillis = 0;

	Button* btn1;
	Button* btn2;
	Button* btn3;
	Button* btn4;

	byte currentClue = 0;
	byte stages = 0;

	byte preStrikeState = 0;

	Adafruit_SSD1306* display;
	SoftwareI2C *displayWire;

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
		
		this->changeStage(STATE_STAGE1);
	}

	void changeStage(byte stage)
	{
		if(stage > this->stages)
		{
			deactivate();
			return;
		}

		this->state = stage;

		this->currentClue = random(0, 4);

		Serial.print("Current stage: ");
		Serial.println(this->state);

		Serial.print("Current clue: ");
		Serial.println(this->currentClue);
	}

	void updateStageDisplay()
	{
		// TODO implement me
	}

	void arm()
	{
		Slave::arm();

		this->state = STATE_STAGE1;
	}

	void strike() override {

		this->preStrikeState = this->state;

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
					this->changeStage(STATE_STAGE2);
				else
					reportStrike();
				break;
			case 2:
				if (btnPressed == 2)
					this->changeStage(STATE_STAGE2);
				else
					reportStrike();
				break;
			case 3:
				if (btnPressed == 3)
					this->changeStage(STATE_STAGE2);
				else
					reportStrike();
				break;
			case 4:
				if (btnPressed == 4)
					this->changeStage(STATE_STAGE2);
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
				this->changeStage(STATE_STAGE3);
			else
				reportStrike();
			break;
		case 2:
			if (btnPressed == 2)
				this->changeStage(STATE_STAGE3);
			else
				reportStrike();
			break;
		case 3:
			if (btnPressed == 1)
				this->changeStage(STATE_STAGE3);
			else
				reportStrike();
			break;
		case 4:
			if (btnPressed == 4)
				this->changeStage(STATE_STAGE3);
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
				this->changeStage(STATE_STAGE4);
			else
				reportStrike();
			break;
		case 2:
			if (btnPressed == 2)
				this->changeStage(STATE_STAGE4);
			else
				reportStrike();
			break;
		case 3:
			if (btnPressed == 3)
				this->changeStage(STATE_STAGE4);
			else
				reportStrike();
			break;
		case 4:
			if (btnPressed == 3)
				this->changeStage(STATE_STAGE4);
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
				this->changeStage(STATE_STAGE5);
			else
				reportStrike();
			break;
		case 2:
			if (btnPressed == 1)
				this->changeStage(STATE_STAGE5);
			else
				reportStrike();
			break;
		case 3:
			if (btnPressed == 1)
				this->changeStage(STATE_STAGE5);
			else
				reportStrike();
			break;
		case 4:
			if (btnPressed == 4)
				this->changeStage(STATE_STAGE5);
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
				this->changeStage(STATE_STAGE6);
			else
				reportStrike();
			break;
		case 2:
			if (btnPressed == 1)
				this->changeStage(STATE_STAGE6);
			else
				reportStrike();
			break;
		case 3:
			if (btnPressed == 1)
				this->changeStage(STATE_STAGE6);
			else
				reportStrike();
			break;
		case 4:
			if (btnPressed == 4)
				this->changeStage(STATE_STAGE6);
			else
				reportStrike();
			break;
		}
	}

	void handleStage6(byte btnPressed)
	{
		switch (this->currentClue)
		{
		case 1:
			if (btnPressed == 2)
				this->deactivate();
			else
				this->reportStrike();
			break;
		case 2:
			if (btnPressed == 1)
				this->deactivate();
			else
				this->reportStrike();
			break;
		case 3:
			if (btnPressed == 1)
				this->deactivate();
			else
				this->reportStrike();
			break;
		case 4:
			if (btnPressed == 4)
				this->deactivate();
			else
				this->reportStrike();
			break;
		}
	}

	void reportStrike() override
	{
		Slave::reportStrike();

		// medium difficulty
		if(this->difficulty == 1 && this->state > 1)
		{
			// drop down a stage
			this->changeStage(this->state - 1);
		}
		// hard difficulty
		else if(this->difficulty == 2 && this->state > 1)
		{
			// go back to the beginning
			this->changeStage(1);
		}
	}

	void updateDisplay()
	{
	}

	void stopStriking()
	{
		this->state = this->preStrikeState;
	}

public:
	Memory() : Slave(4)
	{
		this->btn1 = new Button(9, INPUT_PULLUP);
		this->btn2 = new Button(6, INPUT_PULLUP);
		this->btn3 = new Button(7, INPUT_PULLUP);
		this->btn4 = new Button(8, INPUT_PULLUP);

		this->displayWire = new SoftwareI2C();
		this->displayWire->begin(3, 2);

		this->display = new Adafruit_SSD1306(128, 32, this->displayWire, -1);
		if (!display->begin(SSD1306_SWITCHCAPVCC, 0x3C))
		{ // Address 0x3C for 128x32
			Serial.println(F("SSD1306 allocation failed"));
		}
		//display->display();
		display->clearDisplay();
		display->drawPixel(10, 20, SSD1306_WHITE);
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
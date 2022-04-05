#include <NeedySlave.h>
#include <RotaryEncoder.h>
#include <Adafruit_SSD1306.h>

class Capacitor {

	byte difficulty;

	byte chargeLevelInt = 0;
	float chargeLevel = 0;
	int chargeDeltaInt = 0;
	float chargeDelta = 0;

	byte minStartChargeLevel[3] = { 80, 60, 40 };
	float initialChargeDelta[3] = { -0.1, -0.2, 0.2 };
	byte randomDangerLevel[3] = { 120, 60, 30 };

	Adafruit_SSD1306* display;
	SlowSoftWire *displayWire;

	unsigned long lastFlashMillis = 0;
	unsigned long lastLevelChangeMillis = 0;
	bool flashState = false;
	
	//// Knob details
	RotaryEncoder knob;
	int knobPosition = 0;
	int correctFrequency = 0;
	int currentFrequency = 0;
	const int knobStep[3] = {10, 5, 1};

	void drawScale()
	{
		this->display->setTextSize(1);
		this->display->setTextColor(SSD1306_WHITE);

		this->display->drawRect(0, 10, 128, 11, SSD1306_WHITE);

		this->display->setCursor(0, 0);
		this->display->println(F("0  20  50  80 100 120"));

		this->display->setCursor(32, 32 - 8);
		this->display->println(F("CAPACITOR 1"));

		this->display->display();
	}

	void setChargeDelta(float delta)
	{
		this->chargeDelta = delta;
		this->chargeDeltaInt = this->chargeDelta;
	}

	void handleKnob()
	{
		knob.tick();
		int position = knob.getPosition();
		if (position != knobPosition)
		{
			knobPosition = position;
			this->setChargeDelta(knobPosition == 0 ? -0.1 : knobPosition / 10.0);
		}
	}


	void randomiseChargeLevel()
	{
		byte r = random(0, this->randomDangerLevel[this->difficulty]);
		if(r > 0)
			return;

		float newDelta = this->chargeDelta + this->initialChargeDelta[this->difficulty] * 2;
		Serial.print("Randomising charge delta: ");
		Serial.println(newDelta);

		this->setChargeDelta(newDelta);
		knob.setPosition(0);
	}

	void setNewChargeLevel(float level)
	{
		this->chargeLevel = min(level, 124);
		this->chargeLevelInt = this->chargeLevel;
	}

	void updateDisplay()
	{
		bool showBar = true;

		if(this->chargeLevel < 20 || this->chargeLevel > 100)
		{
			if(!this->flashState)
				showBar = false;
		}

		// remove old bar
		this->display->fillRect(2, 12, 124, 7, SSD1306_BLACK);
		
		// set new bar
		if(showBar)
			this->display->fillRect(2, 12, this->chargeLevel, 7, SSD1306_WHITE);
		
		this->display->display();
	}


public:

	Capacitor(uint8_t sda, uint8_t scl, uint8_t knobA, uint8_t knobB) : 
		knob(knobA, knobB, RotaryEncoder::LatchMode::FOUR3)
	{
		this->displayWire = new SlowSoftWire(sda, scl, true);
		//this->displayWire->setClock(400000);
		this->displayWire->begin();

		this->display = new Adafruit_SSD1306(128, 32, this->displayWire, -1);
		if (!display->begin(SSD1306_SWITCHCAPVCC, 0x3C))
			Serial.println(F("SSD1306 allocation failed"));
		
		this->display->clearDisplay();
		this->drawScale();
	}

	bool chargeLevelOOB()
	{
		return this->chargeLevelInt == 0 || this->chargeLevelInt >= 124;
	}
	
	void reset(uint8_t difficulty)
	{
		this->difficulty = difficulty;
		byte level = random(this->minStartChargeLevel[this->difficulty], 101);
		this->setNewChargeLevel(level);
		this->setChargeDelta(0);
	}

	void activate()
	{
		this->setChargeDelta(this->initialChargeDelta[this->difficulty]);
	}

	void loop()
	{
		unsigned long now = millis();
		if(now > this->lastLevelChangeMillis + 1000)
		{
			this->setNewChargeLevel(this->chargeLevel + this->chargeDelta);
			this->lastLevelChangeMillis = now;

			this->randomiseChargeLevel();
		}

		if(now > this->lastFlashMillis + 800)
		{
			this->flashState = !this->flashState;
			this->lastFlashMillis = now;
			
			this->updateDisplay();
		}

		this->handleKnob();
		
	}
};
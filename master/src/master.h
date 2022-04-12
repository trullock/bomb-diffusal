#include <Arduino.h>
#include <Wire.h>

#include "pins.h"

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

#include <TM1637Display.h>

#include "../lib/constants.h"
#include "module-results.h"
#include "sfx.h"
#include <sounds.h>
#include <serialnumber.h>

#define LOOP_INTERVAL_MS 100
#define BROADCAST 0
#define MASTER_STATE_BOOTING 1
#define MASTER_STATE_ARMED 2
#define MASTER_STATE_EXPLODED 3
#define MASTER_STATE_DEACTIVATED 4

class Master
{
	byte moduleAddresses[16];
	byte moduleCount = 0;

	byte state = 0;
	byte difficulty = 1;
	
	unsigned long prevCountdownMillis = 0;
	uint16_t timeRemainingInS = 0;

	byte totalStrikes = 0;
	byte livesRemaining = 0;
	byte totalDeactivatedModules = 0;

	byte serialNumber[5];

	Adafruit_SSD1306* display;
	Adafruit_AlphaNum4* livesDisplay;
	TM1637Display* timeDisplay;

	unsigned long activateCountdownAtMillis = 0;
	bool countdownActive = false;

	unsigned long prevFlashMillis = 0;
	unsigned long flashDuration = 0;
	unsigned long flashingStartedOnMillis = 0;
	bool flashing = false;
	bool flashOn = false;

	Sfx* sfx;

	byte sendCommand(int address, byte command, byte arg0 = 255, byte arg1 = 255, byte arg2 = 255, byte arg3 = 255, byte arg4 = 255, byte arg5 = 255, byte arg6 = 255, byte arg7 = 255)
	{
		Wire.beginTransmission(address);
		Wire.write(command);

		if(arg0 != 255)
			Wire.write(arg0);
		if(arg1 != 255)
			Wire.write(arg1);
		if(arg2 != 255)
			Wire.write(arg2);
		if(arg3 != 255)
			Wire.write(arg3);
		if(arg4 != 255)
			Wire.write(arg4);
		if(arg5 != 255)
			Wire.write(arg5);
		if(arg6 != 255)
			Wire.write(arg6);
		if(arg7 != 255)
			Wire.write(arg7);

		int error = Wire.endTransmission();
		if (error != 0)
		{
			Serial.print("Error sending command: ");
			Serial.print(command);
			Serial.print(" to address: ");
			Serial.print(address);
			Serial.print(", Error: ");
			Serial.println(error);

			/** 
				 * Errors
				 * 0 .. success
				 * 1 .. length too long for buffer
				 * 2 .. address send, NACK received
				 * 3 .. data send, NACK received
				 * 4 .. other twi error (lost bus arbitration, bus error, ..)
				 * 5 .. timeout
				*/
		}

		return error;
	}

	ModuleResults readModules()
	{
		ModuleResults results;

		for (byte i = 0; i < moduleCount; i++)
		{
			// TODO: can we be less wasteful and not send loads ofs empty sfx queue entries every time?
			unsigned int bytesReceived = Wire.requestFrom((int)moduleAddresses[i], 2 + MODULE_MAX_SFX_QUEUE_LENGTH);

			byte state = Wire.read();
			byte strikes = Wire.read();
			byte sound = 0;
			for(unsigned int i = 2; i < min(bytesReceived, sizeof(results.sounds)); i++)
			{
				sound = Wire.read();
				// if not an I2C error (likely as we're usually asking for more than we'd expect)
				if(sound < 255)
					results.sounds[results.soundCount++] = sound;
			}
			
			// Handle I2C error
			if(state == STATE_NONE)
				continue;

			if (state == STATE_DEACTIVATED)
				results.deactivatedModules++;

			results.strikes += strikes;
		}

		return results;
	}

	void processModules(unsigned long now)
	{
		ModuleResults results = readModules();

		Serial.println("Reading modules");
		Serial.print("	Strikes: ");
		Serial.println(results.strikes);
		Serial.print("	Deactivations: ");
		Serial.println(results.deactivatedModules);

		if (results.strikes != this->totalStrikes)
			this->strike(results.strikes);

		if (results.deactivatedModules != totalDeactivatedModules)
			this->moduleDeactivated(results.deactivatedModules);
		
		for(byte i = 0; i < results.soundCount; i++)
			this->sfx->enqueue(results.sounds[i]);
	}

	void updateCountdown(unsigned long now)
	{
		if(!this->countdownActive)
		{
			if(this->activateCountdownAtMillis != 0 && this->activateCountdownAtMillis <= now)
			{
				this->countdownActive = true;
				this->activateCountdownAtMillis = 0;
			}
			else
			{
				return;
			}
		}

		// Run once a second
		if (now < prevCountdownMillis + 1000)
			return;

		if (this->timeRemainingInS == 0)
			return;

		prevCountdownMillis = now;

		this->timeRemainingInS--;
	
		byte mins = this->timeRemainingInS / 60;
		byte secs = this->timeRemainingInS % 60;

		this->updateTimeDisplay();
		
		// only announce whole mins, 10,20,30,... and 1,2,3,4,5,6,7,8,9
		if(secs == 0 && mins > 0 && (mins % 10 == 0 || mins < 10))
			this->sfx->selfDesctructionIn(mins);

		if(mins == 0)
		{
			// start at 12 as it takes 2s to say "detonation in...", making the "10" land at 10s
			if(secs == 12)
				this->sfx->detonation10sCountdown();
			else if(secs == 0)
				this->explode();
		}
		
		byte arg0 = this->timeRemainingInS >> 8;
		byte arg1 = this->timeRemainingInS;
		sendCommand(BROADCAST, COMMAND_TIME, arg0, arg1);
	}

	void updateLivesDisplay()
	{
		#define LEDSEG_LIFE 0b0011111111111111
		#define LEDSEG_NOLIFE 0b0000000000111111

		this->livesDisplay->writeDigitRaw(0, LEDSEG_NOLIFE);
		this->livesDisplay->writeDigitRaw(1, LEDSEG_NOLIFE);
		this->livesDisplay->writeDigitRaw(2, LEDSEG_NOLIFE);
		this->livesDisplay->writeDigitRaw(3, LEDSEG_NOLIFE);

		if(this->livesRemaining == 4)
			this->livesDisplay->writeDigitRaw(3, LEDSEG_LIFE);
			
		if(this->livesRemaining >= 3)
			this->livesDisplay->writeDigitRaw(2, LEDSEG_LIFE);
			
		if(this->livesRemaining >= 2)
			this->livesDisplay->writeDigitRaw(1, LEDSEG_LIFE);
			
		if(this->livesRemaining >= 1)
			this->livesDisplay->writeDigitRaw(0, LEDSEG_LIFE);
				
		this->livesDisplay->writeDisplay();
	}

	void strike(byte totalStrikes)
	{
		Serial.println("New strike");
		this->totalStrikes = totalStrikes;
		this->livesRemaining--;
		
		// https://learn.adafruit.com/adafruit-led-backpack/0-54-alphanumeric-9b21a470-83ad-459c-af02-209d8d82c462

		this->updateLivesDisplay();

		this->sendCommand(BROADCAST, COMMAND_STRIKE);
		this->sfx->enqueue(Sounds::DeactivationFailure, SFX_ENQUEUE_MODE__INTERRUPT);

		if(this->livesRemaining == 0)
			this->explode();
	}

	void moduleDeactivated(byte totalDeactivatedModules)
	{
		Serial.println("New module deactivated");
		this->totalDeactivatedModules = totalDeactivatedModules;

		if(this->totalDeactivatedModules == this->moduleCount)
			this->deactivate();
		else
			this->sfx->enqueue(Sounds::ComponentDeactivated);
	}

	void deactivate()
	{
		this-> countdownActive = false;
		//this->livesDisplay->clear();
		//this->livesDisplay->writeDisplay();

		this->sfx->enqueue(Sounds::WeaponDeactivated, SFX_ENQUEUE_MODE__INTERRUPT);
		this->sfx->enqueue(Sounds::PowerDown);
		this->state = MASTER_STATE_DEACTIVATED;		
	}

	void explode()
	{
		this->state = MASTER_STATE_EXPLODED;

		this->livesDisplay->clear();
		this->livesDisplay->writeDisplay();

		this->timeDisplay->clear();

		this->countdownActive = false;

		// ~6s is the explosion sound duration
		this->startFlashing(millis(), 6000);

		this->sendCommand(BROADCAST, COMMAND_EXPLODE);
		Master* that = this;
		auto cb = []() {
			//master->test();
			Serial.println("Finished exploding");
		};
		EventCallback<Master>* callback = new EventCallback<Master>(that, &Master::foo);
		this->sfx->enqueue3(Sounds::Explosion, SFX_ENQUEUE_MODE__DEFAULT, callback);
	}

	void foo()
	{
		Serial.print("MEga test");
	}


	void startFlashing(const unsigned long& now, const unsigned long& duration)
	{
		this->flashing = true;
		this->flashDuration = duration;
		this->flashingStartedOnMillis = now;
		this->prevFlashMillis = now;
	}

	void flash(const unsigned long& now)
	{
		if(!this->flashing)
			return;

		if(this->prevFlashMillis + 100 > now)
			return;

		this->prevFlashMillis = now;
		this->flashOn = !this->flashOn;

		this->updateTimeDisplay();
	}

	void updateTimeDisplay()
	{
		if(this->flashing)
		{
			if(this->flashOn)
			{
    			uint8_t data[] = { 255, 255, 255, 255 };
				this->timeDisplay->setSegments(data);
			}
			else
			{
				uint8_t data[] = { 0, 0, 0, 0 };
				this->timeDisplay->setSegments(data);
			}
		}
		else
		{
			byte mins = this->timeRemainingInS / 60;
			byte secs = this->timeRemainingInS % 60;

			int timeAsDec = mins * 100 + secs;
			this->timeDisplay->showNumberDecEx(timeAsDec, 0b01000000, true);
		}
	}

	void scanForModules()
	{
		Serial.println("Scanning for modules...");
		
		byte error;
		for (byte address = 1; address < 15; address++)
		{
			error = this->sendCommand((int)address, COMMAND_SERIAL, this->serialNumber[0], this->serialNumber[1], this->serialNumber[2], this->serialNumber[3], this->serialNumber[4]);
			if (error == 0)
			{
				// if(address == 0x3C)
				// 	continue;

				moduleAddresses[moduleCount] = address;
				moduleCount++;
			}
		}

		Serial.print("	Found ");
		Serial.print(moduleCount);
		Serial.println(" module(s)");

		for(byte i = 0; i < moduleCount; i++)
		{
			Serial.print("	Module found at address 0x");
			if (moduleAddresses[i] < 16)
				Serial.print("0");
			Serial.println(moduleAddresses[i], HEX);
		}
		Serial.println("	Finished");
	}

	void setDifficulty(byte diff)
	{
		// TODO: should we support changing difficulty when armed?
		difficulty = diff;
		sendCommand(BROADCAST, COMMAND_DIFFICULTY, difficulty);

		if(difficulty == 0)
		{
			this->timeRemainingInS = 45 * 60;
			this->livesRemaining = 4;
		}
		else if (difficulty == 1)
		{
			this->timeRemainingInS = 60;//30 * 60;
			this->livesRemaining = 2;
		}
		else if (difficulty == 2)
		{
			this->timeRemainingInS = 20 * 60;
			this->livesRemaining = 1;
		}
	}

public:

	Master()
	{
		Serial.println("Master booting");

		this->state = MASTER_STATE_BOOTING;

		// TODO: is this random enough?
		randomSeed(micros() + analogRead(0));

		generateSerialNumber(this->serialNumber);

		this->display = new Adafruit_SSD1306(128, 64, &Wire);
		if (!this->display->begin(SSD1306_SWITCHCAPVCC, 0x3C, true, false))
			Serial.println(F("SSD1306 allocation failed"));
		
		this->display->clearDisplay();
		this->display->display();

		this->display->setTextSize(2);
		this->display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

		this->livesDisplay = new Adafruit_AlphaNum4();
		this->livesDisplay->begin(0x70);
		
		this->timeDisplay = new TM1637Display(TIME_DISPLAY_CLK, TIME_DISPLAY_DIO);
		this->timeDisplay->setBrightness(7);
		this->timeDisplay->clear();
		
		this->sfx = new Sfx();
		this->sfx->enqueue(Sounds::SystemBootInitiated);
	}

	void arm()
	{
		this->state = MASTER_STATE_ARMED;
		this->sendCommand(BROADCAST, COMMAND_ARM);
		
		this->updateLivesDisplay();

		this->sfx->enqueue(Sounds::WeaponActivated);
		
		byte mins = this->timeRemainingInS / 60;
		this->sfx->selfDesctructionIn(mins);

		// 6s is the approx time the selfDesctruction sfx takes to play
		this->activateCountdownAtMillis = millis() + 6000;
	}

	void boot(uint8_t difficulty)
	{
		this->scanForModules();
		this->setDifficulty(difficulty);
		this->arm();
	}

	void handleModuleInterrupt(unsigned long now)
	{
		if(this->state != MASTER_STATE_ARMED)
			return;

		this->processModules(now);
	}

	void loop(unsigned long now)
	{
		this->sfx->loop();
		
		if(this->state != MASTER_STATE_ARMED)
			return;

		this->updateCountdown(now);
		this->flash(now);
	}

};
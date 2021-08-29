#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "../lib/constants.h"
#include "module-results.h"
#include "sfx.h"
#include <sounds.h>

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
	
	unsigned long lastSecondMillis = 0;
	uint16_t timeRemainingInS = 0;

	byte totalStrikes = 0;
	byte totalDeactivatedModules = 0;

	byte serialNumber[5];

	Adafruit_SSD1306* display;
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

	/**
	 * Read the current status of all known modules
	 */
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

		if (results.strikes != this->totalStrikes)
			this->strike(results.strikes);

		if (results.deactivatedModules != totalDeactivatedModules)
			this->moduleDeactivated(results.deactivatedModules);
		
		for(byte i = 0; i < results.soundCount; i++)
			this->sfx->enqueue(results.sounds[i]);
	}

	void updateCountdown(unsigned long now)
	{
		// Run once a second
		if (now < lastSecondMillis + 1000)
			return;

		if (timeRemainingInS == 0)
			return;

		lastSecondMillis = now;

		timeRemainingInS--;
	
		byte mins = timeRemainingInS / 60;
		byte secs = timeRemainingInS % 60;

		this->display->setCursor(0, 0);
		this->display->print(mins);
		this->display->print(":");
		this->display->print(secs);
		this->display->display();

		// only announce whole mins, 10,20,30,etc and 1,2,3,4,5,6,7,8,9
		if(secs == 0 && mins > 0 && (mins % 10 == 0 || mins < 10))
			this->sfx->selfDesctructionIn(mins);

		if(mins == 0)
		{
			if(secs == 10)
				this->sfx->detonation10sCountdown();
			else if(secs == 0)
				this->explode();
		}
		
		byte arg0 = timeRemainingInS >> 8;
		byte arg1 = timeRemainingInS;
		sendCommand(BROADCAST, COMMAND_TIME, arg0, arg1);
	}

	void strike(byte totalStrikes)
	{
		Serial.println("New strike");
		this->totalStrikes = totalStrikes;
		this->sendCommand(BROADCAST, COMMAND_STRIKE);
		this->sfx->enqueue(Sounds::DeactivationFailure, SFX_ENQUEUE_MODE__INTERRUPT);
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

	void setSerialNumber()
	{
		// TODO: make sure A0 is floating
		randomSeed(micros() + analogReadMilliVolts(A0));

		for(byte i = 0; i < sizeof(this->serialNumber); i++)
			this->serialNumber[i] = random(0, 36);
	}

	void deactivate()
	{
		this->sfx->enqueue(Sounds::WeaponDeactivated, SFX_ENQUEUE_MODE__INTERRUPT);
		this->sfx->enqueue(Sounds::PowerDown);
		this->state = MASTER_STATE_DEACTIVATED;
	}

	void explode()
	{
		this->sendCommand(BROADCAST, COMMAND_EXPLODE);
		this->sfx->enqueue(Sounds::Explosion);
		// TODO: render explosion on master displays
	}

public:

	Master()
	{
		Serial.println("Master booting");

		this->state = MASTER_STATE_BOOTING;

		this->sfx = new Sfx(18, 19, 22);
		this->sfx->enqueue(Sounds::SystemBootInitiated);

		this->setSerialNumber();

		this->display = new Adafruit_SSD1306(128, 64, &Wire);
		if (!this->display->begin(SSD1306_SWITCHCAPVCC, 0x3C, true, false))
			Serial.println(F("SSD1306 allocation failed"));
		
		this->display->clearDisplay();
		this->display->display();

		this->display->setTextSize(2);
		this->display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
	}

	void setDifficulty(byte diff)
	{
		// TODO: should we support changing difficulty when armed?
		difficulty = diff;
		sendCommand(BROADCAST, COMMAND_DIFFICULTY, difficulty);

		if(difficulty == 0)
			timeRemainingInS = 45 * 60;
		else if (difficulty == 1)
			timeRemainingInS = 30;//30 * 60;
		else if (difficulty == 2)
			timeRemainingInS = 20 * 60;
	}

	void arm()
	{
		this->state = MASTER_STATE_ARMED;
		this->sendCommand(BROADCAST, COMMAND_ARM);

		this->sfx->enqueue(Sounds::WeaponActivated);
		
		byte mins = this->timeRemainingInS / 60;
		this->sfx->selfDesctructionIn(mins);
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
	}
};
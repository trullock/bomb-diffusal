#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "../lib/constants.h"
#include "module-results.h"
#include "sfx.h"
#include "../lib/sounds.h"

#define LOOP_INTERVAL_MS 100

class Master
{
	byte moduleAddresses[16];
	byte moduleCount = 0;

	bool armed = false;
	byte difficulty = 1;
	
	unsigned long lastLoopMillis = 0;
	unsigned long lastSecondMillis = 0;
	unsigned int timeRemainingInS = 0;

	byte strikes = 0;
	byte deactivatedModules = 0;

	Adafruit_SSD1306* display;
	Sfx* sfx;

	void sendCommand(byte command, byte arg)
	{
		if(this->moduleCount == 0)
			return;
			
		
		Wire.beginTransmission(0);
		Wire.write(command);
		if(arg != 0)
			Wire.write(arg);

		int error = Wire.endTransmission();
		if (error != 0)
		{
			Serial.print("Error sending command: ");
			Serial.print(command);
			Serial.print(", Error: ");
			Serial.println(error);

			/** 
				 * Errors
				 * 0 .. success
				 * 1 .. length to0 long for buffer
				 * 2 .. address send, NACK received
				 * 3 .. data send, NACK received
				 * 4 .. other twi error (lost bus arbitration, bus error, ..)
				 * 5 .. timeout
				*/
		}
	}

	void sendCommand(byte command, unsigned int arg)
	{
		if(this->moduleCount == 0)
			return;

		Wire.beginTransmission(0);
		Wire.write(command);
		if(arg != 0)
			Wire.write(arg);

		int error = Wire.endTransmission();
		if (error != 0)
		{
			Serial.print("Error sending command: ");
			Serial.print(command);
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
	}

	void sendCommand(byte command)
	{
		sendCommand(command, (byte)0);
	}

	/**
	 * Read the current status of all known modules
	 */
	ModuleResults readModules()
	{
		ModuleResults results;

		for (byte i = 0; i < moduleCount; i++)
		{
			Wire.requestFrom((int)moduleAddresses[i], 3);
			byte state = Wire.read();
			byte strikes = Wire.read();
			byte notification = Wire.read();

			// Handle I2C error
			if(state == STATE_NONE)
				continue;

			if (state == STATE_DEACTIVATED)
				results.deactivatedModules++;

			results.strikes += strikes;
			results.notification |= notification;
		}

		return results;
	}


	void scanForModules()
	{
		Serial.println("Scanning for modules...");
		byte error;
		for (byte address = 1; address < 15; address++)
		{
			Wire.beginTransmission(address);
			error = Wire.endTransmission();

			if (error == 0)
			{
				// DONT uncomment these, it creates all kinds of wierd multi-threading looking issues which are unexplainable. Winner.

				//Serial.print("	Module found at address 0x");
				//if (address < 16)
				//	Serial.print("0");
				//Serial.println(address, HEX);

				if(address == 0x3C)
				{
					//Serial.println("		Skipping");
					continue;
				}

				moduleAddresses[moduleCount] = address;
				moduleCount++;
			}
			else if (error == 4)
			{
				Serial.print("	Unknown error at address 0x");
				if (address < 16)
					Serial.print("0");
				Serial.println(address, HEX);
			}
		}

		Serial.print("	Found ");
		Serial.print(moduleCount);
		Serial.println(" module(s)");
		Serial.println("	Finished");
	}

	void updateCountdown(unsigned long now)
	{
		if (now > lastSecondMillis + 1000)
		{
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
			if(secs == 0 && (mins % 10 == 0 || mins < 10))
				this->sfx->selfDesctructionIn(mins);

			// 10s remaining countdown
			if(mins == 0 && secs == 10)
				this->sfx->detonation10sCountdown();

			sendCommand(COMMAND_TIME, timeRemainingInS);
		}
	}

	void strike(byte strikes)
	{
		Serial.println("New strike");
		this->strikes = strikes;
		this->sendCommand(COMMAND_STRIKE);
		this->sfx->enqueue(Sounds::DeactivationFailure);
	}

	void moduleDeactivated(byte modules)
	{
		Serial.println("New module deactivated");
		this->deactivatedModules = modules;
		this->sfx->enqueue(Sounds::ComponentDeactivated);
	}

	void processModules(unsigned long now)
	{
		ModuleResults results = readModules();

		if (results.strikes != this->strikes)
			this->strike(results.strikes);

		if (results.deactivatedModules != deactivatedModules)
			this->moduleDeactivated(results.deactivatedModules);
		
		if (results.notification != 0)
			this->sfx->enqueue(results.notification);
	}

public:

	Master()
	{
		Serial.println("Master booting");

		// TODO: can we use any pins?
		this->sfx = new Sfx(27, 26, 25);
		this->sfx->enqueue(Sounds::SystemBootInitiated);

		this->display = new Adafruit_SSD1306(128, 64, &Wire);
		if (!this->display->begin(SSD1306_SWITCHCAPVCC, 0x3C, true, false))
			Serial.println(F("SSD1306 allocation failed"));
		
		this->display->clearDisplay();
		this->display->display();

		this->display->setTextSize(2);
		this->display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

		this->scanForModules();
	}

	void setDifficulty(byte diff)
	{
		// TODO: should we support changing difficulty when armed?
		difficulty = diff;
		sendCommand(COMMAND_DIFFICULTY, difficulty);

		if(difficulty == 0)
			timeRemainingInS = 45 * 60;
		else if (difficulty == 1)
			timeRemainingInS = 30 * 60;
		else if (difficulty == 2)
			timeRemainingInS = 20 * 60;
	}

	void arm()
	{
		this->armed = true;
		this->sendCommand(COMMAND_ARM);

		this->sfx->enqueue(Sounds::WeaponActivated);
		
		byte mins = this->timeRemainingInS / 60;
		this->sfx->selfDesctructionIn(mins);
	}

	void loop()
	{
		this->sfx->loop();
		
		if(!this->armed)
			return;

		unsigned long now = millis();

		// Dont hammer the slaves, give them some breathing room
		if(now < this->lastLoopMillis + LOOP_INTERVAL_MS)
			return;
		
		this->processModules(now);
		
		this->updateCountdown(now);

		this->lastLoopMillis = now;
	}
};
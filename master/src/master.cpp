#include <Arduino.h>
#include <SD.h>
#include <TMRpcm.h>
#include <Wire.h>


#include "../lib/constants.h"
#include "module-results.h"

#define SPEAKER_PIN 9
#define SD_CHIPSELECT_PIN 4

class Master
{
	// https://github.com/Koepel/How-to-use-the-Arduino-Wire-library/wiki

	byte moduleAddresses[127];
	byte moduleCount = 0;

	bool armed = false;
	byte difficulty = 1;
	
	unsigned long lastSecondMillis = 0;
	unsigned int timeRemainingInS = 0;
	byte timeRemainingInM = 0;

	byte strikes = 0;
	byte deactivatedModules = 0;

	TMRpcm tmrpcm;

	void sendCommand(byte command, byte arg)
	{
		Wire.beginTransmission(0);
		Wire.write(command);
		if(arg != 0)
			Wire.write(arg);

		int error = Wire.endTransmission();
		if (error != 0)
		{
			Serial.print("Error sending command: ");
			Serial.println(error);
		}
	}

	void sendCommand(byte command)
	{
		sendCommand(command, 0);
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
		for (byte address = 1; address < 127; address++)
		{
			Wire.beginTransmission(address);
			error = Wire.endTransmission();

			if (error == 0)
			{
				Serial.print("	Module found at address 0x");
				if (address < 16)
					Serial.print("0");
				Serial.println(address, HEX);

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
			byte timeInM = ceil(timeRemainingInS / 60.0);

			// TODO: update counter

			if(timeRemainingInM != timeInM)
			{
				timeRemainingInM = timeInM;
				sendCommand(COMMAND_TIME, timeInM);
			}
		}
	}

	void strike()
	{
		Serial.println("New strike");
		this->sendCommand(COMMAND_STRIKE);
		this->tmrpcm.play("strike.wav");
	}

	void moduleDeactivated()
	{
		Serial.println("New module deactivated");
	}

	void handleNotifications(byte notifications)
	{

	}

public:

	Master()
	{
		tmrpcm.speakerPin = SPEAKER_PIN;
		if (!SD.begin(SD_CHIPSELECT_PIN))
			Serial.println("SD fail");

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
		if (difficulty == 2)
			timeRemainingInS = 20 * 60;
	}

	void arm()
	{
		this->armed = true;
		this->sendCommand(COMMAND_ARM);
	}

	void loop(unsigned long now)
	{
		ModuleResults results = readModules();

		if (results.strikes != this->strikes)
			this->strike();

		if (results.deactivatedModules != deactivatedModules)
			this->moduleDeactivated();
		
		if (results.notification != 0)
			this->handleNotifications(results.notification);

		this->updateCountdown(now);

		this->strikes = results.strikes;
		this->deactivatedModules = results.deactivatedModules;
	}
};
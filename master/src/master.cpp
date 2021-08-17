#include <Arduino.h>
#include <Wire.h>

#include "../lib/constants.h"
#include "module-results.h"

#define LOOP_INTERVAL_MS 100


class Master
{
	// https://github.com/Koepel/How-to-use-the-Arduino-Wire-library/wiki

	byte moduleAddresses[127];
	byte moduleCount = 0;

	bool armed = false;
	byte difficulty = 1;
	
	unsigned long lastLoopMillis = 0;
	unsigned long lastSecondMillis = 0;
	unsigned int timeRemainingInS = 0;

	byte strikes = 0;
	byte deactivatedModules = 0;

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

			timeRemainingInS--;
			
			// TODO: update screen
				
			Serial.print("Sending Time-remaining command: ");
			Serial.print(timeRemainingInS);
			Serial.println("s");

			sendCommand(COMMAND_TIME, timeRemainingInS);
		}
	}

	void strike()
	{
		Serial.println("New strike");
		this->sendCommand(COMMAND_STRIKE);
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
		Serial.println("Master booting");

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
		// Dont hammer the slaves, give them some breathing room
		if(now < this->lastLoopMillis + LOOP_INTERVAL_MS)
			return;
			
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

		this->lastLoopMillis = now;
	}
};
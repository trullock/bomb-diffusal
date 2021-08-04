#include <Wire.h>
#include <SD.h>
#include "../lib/TMRpcm-1.2.0/TMRpcm.h"
#include "../lib/constants.h"

// https://github.com/Koepel/How-to-use-the-Arduino-Wire-library/wiki

byte moduleAddresses[127];
byte moduleCount = 0;

byte difficulty = 1;
unsigned int timeRemainingInS = 0;
byte timeRemainingInM = 0;

byte strikes = 0;
byte deactivatedModules = 0;

#define SD_ChipSelectPin 4
TMRpcm tmrpcm;

#define MODULE_READ_INTERVAL_MS 100
unsigned long lastReadMillis = 0;
unsigned long lastSecondMillis = 0;

struct ModuleResults
{
	byte strikes = 0;
	byte deactivatedModules = 0;
	byte notification = 0;
};

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

ModuleResults readModules()
{
	ModuleResults results;

	for (byte i = 0; i < moduleCount; i++)
	{
		Wire.requestFrom((int)moduleAddresses[i], 3);
		byte state = Wire.read();
		byte strikes = Wire.read();
		byte notification = Wire.read();

		if(state == STATE_NONE)
			continue;

		if (state == STATE_DEACTIVATED)
			results.deactivatedModules++;

		results.strikes += strikes;
		results.notification |= notification;
	}

	return results;
}

void scanI2C()
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

void setDifficulty(byte diff)
{
	difficulty = diff;
	sendCommand(COMMAND_DIFFICULTY, difficulty);

	if(difficulty == 0)
		timeRemainingInS = 45 * 60;
	else if (difficulty == 1)
		timeRemainingInS = 30 * 60;
	if (difficulty == 2)
		timeRemainingInS = 20 * 60;
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

void setup()
{
	Serial.println("Master booting");

	Wire.begin();
	Serial.begin(9600);

	tmrpcm.speakerPin = 9;
	if (!SD.begin(SD_ChipSelectPin))
		Serial.println("SD fail");

	//tmrpcm.play("music");

	// let modules boot first
	delay(100);

	scanI2C();

	sendCommand(COMMAND_ARM);

}


void loop()
{
	unsigned long now = millis();

	// Dont hammer the slaves, give them some breathing room
	if(now < lastReadMillis + MODULE_READ_INTERVAL_MS)
		return;

	ModuleResults results = readModules();

	if (results.strikes != strikes)
	{
		// new strike
		Serial.println("New strike");
		sendCommand(COMMAND_STRIKE);
		tmrpcm.play("strike.wav");
	}

	updateCountdown(now);

	if (results.deactivatedModules != deactivatedModules)
	{
		// new module disarmed
		Serial.println("New module deactivated");
	}

	if (results.notification != 0)
	{
	}

	strikes = results.strikes;
	deactivatedModules = results.deactivatedModules;

	lastReadMillis = now;
}

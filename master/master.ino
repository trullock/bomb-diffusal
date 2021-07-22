#include <Wire.h>
#include "SD.h"
#include "TMRpcm.h"
#include "SPI.h"


byte moduleAddresses[127];
int nModules = 0;

byte strikes = 0;
byte disarmedModules = 0;

#define STATUS_ARMED 1
#define STATUS_DISARMED 2

#define COMMAND_ARM 1
#define COMMAND_DIFFICULTY 2

#define DIFFICULTY_LOW 1
#define DIFFICULTY_MED 2
#define DIFFICULTY_HIGH 3


#define SD_ChipSelectPin 4
TMRpcm tmrpcm;


struct ModuleResults
{
	byte strikes = 0;
	byte disarmedModules = 0;
	byte notification = 0;
};

void setup()
{
	Serial.println("Master booting");

	Wire.begin();
	Serial.begin(9600);
	
	tmrpcm.speakerPin = 9;
	if (!SD.begin(SD_ChipSelectPin))
		Serial.println("SD fail");
	

	tmrpcm.play("music");

	// let modules boot first
	delay(100);

	scanI2C();
	setDifficulty(1);
	arm();
}

void setDifficulty(byte difficulty)
{
	Wire.beginTransmission(0);
	Wire.write(COMMAND_DIFFICULTY);
	Wire.write(difficulty);
	int error = Wire.endTransmission();
	if(error != 0)
	{
		Serial.print("Error sending set-difficulty command: ");
		Serial.println(error);
	}
}


void arm()
{
	Wire.beginTransmission(0);
	Wire.write(COMMAND_ARM);
	int error = Wire.endTransmission();
	if(error != 0)
	{
		Serial.print("Error sending arm command: ");
		Serial.println(error);
	}
}

void loop()
{
	ModuleResults results = readModules();

	if(results.strikes != strikes)
	{
		// new strike
		Serial.println("New strike");
	}

	if(results.disarmedModules != disarmedModules)
	{
		// new module disarmed
		Serial.println("New module disarming");
	}

	if(results.notification != 0)
	{

	}

	strikes = results.strikes;
	disarmedModules = results.disarmedModules;
}



ModuleResults readModules()
{
	ModuleResults results;

	for(byte i = 0; i < nModules; i++)
	{
		Wire.requestFrom((int)moduleAddresses[i], 2);
		byte status = Wire.read();
		byte strikes = Wire.read();
		byte notification = Wire.read();

		if((status & STATUS_DISARMED) == STATUS_DISARMED)
			results.disarmedModules++;

		results.strikes += strikes;
		results.notification |= notification;
	}

	return results;
}

void scanI2C() {
	Serial.println("Scanning for modules...");
	byte error;
	for(byte address = 1; address < 127; address++) 
	{
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0)
		{
			Serial.print("	Module found at address 0x");
			if (address<16) 
				Serial.print("0");
			Serial.println(address, HEX);

			moduleAddresses[nModules] = address;
			nModules++;
		}
		else if (error == 4) 
		{
			Serial.print("	Unknown error at address 0x");
			if (address<16) 
				Serial.print("0");
			Serial.println(address, HEX);
		}    
	}

	Serial.print("	Found ");
	Serial.print(nModules);
	Serial.println(" module(s)");
	Serial.println("	Finished");
}
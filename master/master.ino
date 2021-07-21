#include <Wire.h>

byte devices[127];
int nDevices = 0;

byte strikes = 0;
byte disarmedModules = 0;

#define STATUS_ARMED 1
#define STATUS_DISARMED 2

struct ModuleResults
{
	byte strikes = 0;
	byte disarmedModules = 0;
};

void setup()
{
	Wire.begin();
	Serial.begin(9600);
	
	scanI2C();
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

	strikes = results.strikes;
	disarmedModules = results.disarmedModules;
}

ModuleResults readModules()
{
	ModuleResults results;

	for(byte i = 0; i < nDevices; i++)
	{
		Wire.requestFrom(devices[i], 2);
		byte status = Wire.read();
		byte strikes = Wire.read();

		if((status & STATUS_DISARMED) == STATUS_DISARMED)
			results.disarmedModules++;

		results.strikes += strikes;
	}

	return results;
}

void scanI2C() {
	byte error;
	for(byte address = 1; address < 127; address++) 
	{
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0)
		{
			Serial.print("I2C device found at address 0x");
			if (address<16) 
				Serial.print("0");
			Serial.println(address, HEX);

			devices[nDevices] = address;
			nDevices++;
		}
		else if (error == 4) 
		{
			Serial.print("Unknown error at address 0x");
			if (address<16) 
				Serial.print("0");
			Serial.println(address,HEX);
		}    
	}
}
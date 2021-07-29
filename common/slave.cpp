#include <Wire.h>
#include <Arduino.h>
#include "slave.h"
#include "messages.h"

Slave::Slave(int i2cAddress) {
	self = this;
	this->i2cAddress = i2cAddress;

	Wire.begin(this->i2cAddress);
	// enable broadcasts to be received
	TWAR = (this->i2cAddress << 1) | 1;

	Wire.onRequest(Slave::reportStatusWrapper);
	Wire.onReceive(Slave::receiveCommandWrapper);

	Serial.print("Module joining I2C bus with address: ");
	Serial.println(this->i2cAddress);
}

void Slave::reportStatusWrapper() {
	self->reportStatus();
}

void Slave::receiveCommandWrapper(int x) {
	self->receiveCommand(x);
}

void Slave::reportStatus() {
	Wire.write(status);
	Wire.write(strikes);
	Wire.write(pendingNotification);
	pendingNotification = 0;
}

void Slave::arm()
{
	armed = true;
}

void Slave::explode()
{
	// TODO:
}

void Slave::receiveCommand (int howMany)
{
	// Master scanning for modules will cause this
	if(howMany == 0)
		return;
	
	byte command = Wire.read();

	if(command == COMMAND_DIFFICULTY)
	{
		difficulty = Wire.read();
		
		Serial.print("Setting difficulty to: ");
		Serial.println(difficulty);

		if(armed)
			arm();
	}
	else if(command == COMMAND_ARM)
	{
		Serial.println("Arming");
		arm();
	}
	else if(command == COMMAND_EXPLODE)
	{
		Serial.println("Exploding")
		explode();
	}
}

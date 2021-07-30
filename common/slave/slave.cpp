#include <Wire.h>
#include <Arduino.h>
#include "slave.h"

Slave* Slave::self = nullptr;

Slave::Slave(int i2cAddress) {
	self = this;
	this->i2cAddress = i2cAddress;

	Wire.begin(this->i2cAddress);
	// enable broadcasts to be received
	TWAR = (this->i2cAddress << 1) | 1;

	memset(this->commandBuffer, 0, 32);

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
	
	Wire.write(this->state);
	Wire.write(this->strikes);
	Wire.write(this->pendingNotification);
	this->pendingNotification = 0;
}

void Slave::reportStrike()
{
	Serial.println("Reporting strike");
	// The master will query this and relay it back as a Strike() instruction
	this->strikes++;
}

void Slave::arm()
{
	Serial.println("Arming");
}

void Slave::explode()
{
	Serial.println("Exploding");
	this->state = STATE_EXPLODING;
}

void Slave::strike()
{
	Serial.println("Striking");
	this->state = STATE_STRIKING;
}

void Slave::deactivate()
{
	Serial.println("Deactivated");
	this->state = STATE_DEACTIVATED;
}

void Slave::setDifficulty(byte diff)
{
	Serial.print("Setting difficulty to: ");
	Serial.println(diff);

	this->difficulty = diff;
}

void Slave::receiveCommand(int howMany)
{
	// Master scanning for modules will cause this
	if(howMany == 0)
		return;

	this->commandBuffer[this->commandBufferLength] = Wire.read();

	if(howMany == 2)
		this->commandBuffer[this->commandBufferLength + 1] = Wire.read();

	this->commandBufferLength += 2;
}

void Slave::handleCommand()
{
	if (this->commandBufferLength == 0)
		return;

	for(byte i = 0; i < this->commandBufferLength; i += 2)
	{
		byte command = this->commandBuffer[i];
		byte data = this->commandBuffer[i + 1];

		Serial.print("Received Command: ");
		Serial.println(command);

		if (command == COMMAND_DIFFICULTY)
			this->setDifficulty(data);
		else if (command == COMMAND_ARM)
			this->arm();
		else if (command == COMMAND_STRIKE)
			this->strike();
		else if (command == COMMAND_EXPLODE)
			this->explode();
	}

	this->commandBufferLength = 0;
}

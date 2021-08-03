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

void Slave::loop()
{
	this->handleCommand();
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

	if(commandBufferLength == 8)
	{
		// If this buffer is full we have real problems, probably a bug somewhere else.
		return;
	}

	this->commandBuffer[this->commandBufferLength] = Wire.read();
	this->commandBufferLength++;
	
	if (howMany == 2)
		this->commandBuffer[this->commandBufferLength] = Wire.read();
	else
		this->commandBuffer[this->commandBufferLength] = 0;

	this->commandBufferLength++;
}

void Slave::handleCommand()
{
	byte buffer[8];
	byte length = 0;

	noInterrupts();
	length = commandBufferLength;
	commandBufferLength = 0;
	for (byte i = 0; i < length; i++)
		buffer[i] = this->commandBuffer[i];
	interrupts();

	if (length == 0)
		return;

	for (byte i = 0; i < length; i += 2)
	{
		byte command = buffer[i];
		byte data = buffer[i + 1];

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
}

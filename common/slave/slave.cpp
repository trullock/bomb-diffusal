#include <Wire.h>
#include <Arduino.h>
#include "slave.h"

Slave* Slave::self = nullptr;

Slave::Slave(uint8_t i2cAddress, uint8_t raiseInterruptPin)
{
	self = this;

	memset(this->serialNumber, 0, sizeof(this->serialNumber));

	this->raiseInterruptPin = raiseInterruptPin;
	this->interrupting = false;
	pinMode(raiseInterruptPin, OUTPUT);

	Wire.begin(i2cAddress);
	// enable broadcasts to be received
	TWAR = (i2cAddress << 1) | 1;
	Wire.onRequest(Slave::reportStatusWrapper);
	Wire.onReceive(Slave::receiveCommandWrapper);

	Serial.print("Joining I2C bus with address: ");
	Serial.println(i2cAddress);
}

void Slave::reportStatusWrapper()
{
	self->reportStatus();
}

void Slave::receiveCommandWrapper(int x)
{
	self->receiveCommand(x);
}

void Slave::raiseMasterInterrupt()
{
	this->interrupting = true;
	digitalWrite(this->raiseInterruptPin, HIGH);
}

void Slave::endMasterInterrupt(){
	this->interrupting = false;
	digitalWrite(this->raiseInterruptPin, LOW);
}

void Slave::reportStatus()
{
	Wire.write(this->state);
	Wire.write(this->strikes);
	for(byte i = 0; i < this->sfxQueueLength; i++)
		Wire.write(this->sfxQueue[i]);
	this->sfxQueueLength = 0;
}

void Slave::updateTimeRemaining(uint16_t secs)
{
	this->timeRemainingInSecs = secs;
}

void Slave::reportStrike()
{
	Serial.println("Reporting strike");
	// The master will query this and relay it back as a Strike() instruction
	this->strikes++;
	this->raiseMasterInterrupt();

	#ifdef DEVMODE
		this->strike();
	#endif
}

void Slave::loop()
{
	// always end this on every loop. Clock speed of Master and Slave should be sufficient 
	// for Master to have noticed the interrupt before the Slave loops
	this->endMasterInterrupt();

	this->handleCommand();
}

void Slave::arm()
{
	Serial.println("Arming");
	this->strikes = 0;
	// State set by sub-classes
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
	if(this->state == STATE_DEACTIVATED)
		return;
	
	Serial.println("Deactivated");
	this->state = STATE_DEACTIVATED;
	this->raiseMasterInterrupt();
}

bool Slave::queueSfx(byte sound)
{
	if(this->sfxQueueLength == sizeof(this->sfxQueue) - 1)
		return false;

	Serial.print("Playing Sfx: ");
	Serial.println(sound);

	this->sfxQueue[this->sfxQueueLength++] = sound;
	return true;
}

void Slave::setSerialNumber(byte a, byte b, byte c, byte d, byte e)
{
	this->serialNumber[0] = a;
	this->serialNumber[1] = b;
	this->serialNumber[2] = c;
	this->serialNumber[3] = d;
	this->serialNumber[4] = e;
}

void Slave::setDifficulty(byte diff)
{
	Serial.print("Setting difficulty to: ");
	Serial.println(diff);

	this->difficulty = diff;
}

void Slave::receiveCommand(int howMany)
{
	if(commandBufferLength == 12)
	{
		// If this buffer is full we have real problems, probably a bug somewhere else.
		return;
	}

	for(int i = 0; i < howMany; i++)
		this->commandBuffer[this->commandBufferLength++] = Wire.read();
}

void Slave::handleCommand()
{
	byte buffer[12];
	byte length = 0;

	noInterrupts();
	length = commandBufferLength;
	commandBufferLength = 0;
	for (byte i = 0; i < length; i++)
		buffer[i] = this->commandBuffer[i];
	interrupts();

	if (length == 0)
		return;

	for (byte i = 0; i < length;)
	{
		byte command = buffer[i];

		Serial.print("Received Command: ");
		Serial.println(command);

		if (command == COMMAND_DIFFICULTY)
		{
			this->setDifficulty(buffer[i + 1]);
			i += 2;
		}
		else if (command == COMMAND_ARM)
		{
			this->arm();
			i++;
		}
		else if (command == COMMAND_STRIKE)
		{
			this->strike();
			i++;
		}
		else if (command == COMMAND_EXPLODE)
		{
			this->explode();
			i++;
		}
		else if (command == COMMAND_TIME)
		{
			uint16_t time = (buffer[i + 1] << 8) + buffer[i + 2];
			this->updateTimeRemaining(time);
			i+=3;
		}
		else if (command == COMMAND_SERIAL)
		{
			this->setSerialNumber(buffer[i + 1], buffer[i + 2], buffer[i + 3], buffer[i + 4], buffer[i + 5]);
			i+=6;
		}
		else
		{
			// received something we dont understand
			break;
		}
	}
}

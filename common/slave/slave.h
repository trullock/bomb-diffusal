#include <Arduino.h>
#include <Wire.h>
#include "../constants.h"

class Slave {

	static Slave* self;
	static void reportStatusWrapper();
	static void receiveCommandWrapper(int x);

	void reportStatus();
	void receiveCommand(int howMany);
	void endMasterInterrupt();

	volatile byte commandBuffer[12];
	volatile byte commandBufferLength = 0;
	
	byte i2cAddress = 0;
	unsigned int raiseInterruptPin;
	bool interrupting;

	volatile byte sfxQueue[MODULE_MAX_SFX_QUEUE_LENGTH];
	volatile byte sfxQueueLength = 0;

protected:

	byte strikes = 0;
	byte state = STATE_DISARMED;
	byte difficulty = 1;
	byte timeRemainingInSecs = 0;

	void handleCommand();

	void raiseMasterInterrupt();

	virtual void updateTimeRemaining(unsigned int secs);
	virtual void setDifficulty(byte difficulty);

	virtual void arm();
	virtual void strike();
	virtual void explode();
	virtual void deactivate();

	virtual void reportStrike();
	virtual void playSfx(byte sound);

public:

	Slave(byte i2cAddress);
	Slave(byte i2cAddress, uint8_t raiseInterruptPin);
	virtual void loop();
};
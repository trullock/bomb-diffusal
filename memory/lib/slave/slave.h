#include <Arduino.h>
#include <Wire.h>
#include "../constants.h"

class Slave {

	static Slave* self;
	static void reportStatusWrapper();
	static void receiveCommandWrapper(int x);

	void reportStatus();
	void receiveCommand(int howMany);

	volatile byte commandBuffer[32];
	volatile byte commandBufferLength = 0;

protected:
	int i2cAddress = 0;

	byte pendingNotification = 0;
	byte strikes = 0;
	byte state = STATE_DISARMED;
	byte difficulty = 1;

	void handleCommand();

	virtual void reportStrike();

	virtual void arm();
	virtual void strike();
	virtual void explode();
	virtual void deactivate();
	virtual void setDifficulty(byte difficulty);

public:

	Slave(int i2cAddress);
	virtual void loop();
};
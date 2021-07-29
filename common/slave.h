#include <Wire.h>
#include <Arduino.h>

class Slave {

	static Slave* self;
	static void reportStatusWrapper();
	static void receiveCommandWrapper(int x);

	void reportStatus();
	void receiveCommand(int howMany);

protected:
	int i2cAddress = 0;

	byte pendingNotification = 0;
	byte strikes = 0;
	byte status = 0;
	
	bool armed = false;
	byte difficulty = 1;

	bool deactivated = false;

	virtual void reportStrike();

public:

	Slave(int i2cAddress);

	virtual void arm();
	virtual void explode();	
};
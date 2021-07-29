#include <Wire.h>
#include <Arduino.h>

class Slave {

	static Slave* self;
	static void reportStatusWrapper();
	static void receiveCommandWrapper(int x);

protected:
	int i2cAddress = 0;

	byte pendingNotification = 0;
	byte strikes = 0;
	byte status = 0;
	
	bool armed = false;
	byte difficulty = 1;

	bool deactivated = false;

public:

	Slave(int i2cAddress);

	void reportStatus();
	void receiveCommand (int howMany);

	virtual void arm();
	virtual void explode();	
}
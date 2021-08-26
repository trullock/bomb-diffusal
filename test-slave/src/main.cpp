#include "test-slave.cpp"

TestSlave* slave;

void setup()
{
	Wire.begin();
	Serial.begin(9600);

	slave = new TestSlave();
}

void loop()
{
	slave->loop();
}
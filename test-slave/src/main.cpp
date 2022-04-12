#include "test-slave.cpp"

TestSlave* slave;

void setup()
{
	Wire.begin();
	Serial.begin(115200);

	slave = new TestSlave();
}

void loop()
{
	slave->loop();
}
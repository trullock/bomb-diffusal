#include "capacitors.cpp"

Capacitors* capactiors;

void setup()
{
	Wire.begin();
	Serial.begin(9600);

	capactiors = new Capacitors();
}


void loop()
{
	capactiors->loop();
}
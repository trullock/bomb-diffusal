#include <Arduino.h>
#include "morse.cpp"

Morse* morse;

void setup()
{
	Serial.begin(9600);

	morse = new Morse();
}

void loop()
{
	morse->loop();
}
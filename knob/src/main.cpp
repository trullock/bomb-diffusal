#include <Arduino.h>
#include "knob.cpp"

Knob* knob;

void setup()
{
	Serial.begin(9600);

	knob = new Knob();
}

void loop()
{
	knob->loop();
}
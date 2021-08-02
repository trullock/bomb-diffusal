#include <Arduino.h>
#include "memory.cpp"

Memory* memory;

void setup()
{
	Serial.begin(9600);

	memory = new Memory();
}

void loop()
{
	memory->loop();
}
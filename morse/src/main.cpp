#include <Arduino.h>
#include "morse.cpp"
#include <SPI.h>

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
#include <Arduino.h>
#include "combolock.h"
#include <SPI.h>

Combolock* lock;

void setup()
{
	Serial.begin(9600);
	
	lock = new Combolock();
}

void loop()
{
	lock->loop();
}
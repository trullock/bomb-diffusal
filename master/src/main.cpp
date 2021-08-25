#include <Arduino.h>
#include <Wire.h>
#include <SD.h>
#include "master.cpp"

#include "../lib/button.h"

Master* master;
Button btnDifficulty0(7, INPUT_PULLUP);
Button btnDifficulty1(8, INPUT_PULLUP);
Button btnDifficulty2(9, INPUT_PULLUP);
volatile bool moduleInterruptRequest;
bool booted = false;

void IRAM_ATTR moduleISR()
{
	moduleInterruptRequest = true;
}


void setup()
{
	Wire.begin(4, 21);
	Serial.begin(9600);

	moduleInterruptRequest = false;
	attachInterrupt(23, moduleISR, RISING);

	master = new Master();
}

void loop()
{
	// TODO: should we support ad hoc difficulty changing?
	// if(btnDifficulty0.pressed())
	// 	master->setDifficulty(0);
	// else if(btnDifficulty1.pressed())
	// 	master->setDifficulty(1);
	// else if(btnDifficulty2.pressed())
	// 	master->setDifficulty(2);

	unsigned long now = millis();

	// wait 2s for all modules to have booted
	if(!booted && now > 2000)
	{
		booted = true;
		master->scanForModules();
		master->setDifficulty(1);
		master->arm();
	}

	if(moduleInterruptRequest)
	{
		master->processModules(now);
		moduleInterruptRequest = false;
	}

	master->loop(now);
}

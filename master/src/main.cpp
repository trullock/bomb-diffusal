#include <Arduino.h>
#include <Wire.h>
#include "master.h"
#include "pins.h"

#include "../lib/button.h"

Master* master;
Button btnDifficulty0(5, INPUT_PULLUP);
Button btnDifficulty1(18, INPUT_PULLUP);
Button btnDifficulty2(19, INPUT_PULLUP);

volatile bool moduleInterruptRequest;
bool booted = false;
unsigned long lastMillis;

void IRAM_ATTR moduleISR()
{
	moduleInterruptRequest = true;
}

void setup()
{
	Wire.begin(I2C_DATA, I2C_CLK);
	Serial.begin(115200);

	moduleInterruptRequest = false;
	pinMode(MODULE_INTERRUPT, INPUT);
	attachInterrupt(MODULE_INTERRUPT, moduleISR, RISING);

	master = new Master();

	lastMillis = millis();
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



	uint8_t difficulty = 1;

	unsigned long now = millis();

	// wait some time for all modules to have booted + let the Booting sound play, as scanForModules is blocking and breaks the audio
	if(!booted && now > 2500 + lastMillis)
	{
		booted = true;
		master->boot(difficulty);
	}

	if(booted && moduleInterruptRequest)
	{
		Serial.println("Module interrupt");
		master->handleModuleInterrupt(now);
		moduleInterruptRequest = false;
	}

	master->loop(now);
}

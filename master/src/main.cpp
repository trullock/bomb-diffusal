#include <Arduino.h>
#include <Wire.h>
#include "master.cpp"

#include "../lib/button.h"

#define LOOP_INTERVAL_MS 100
unsigned long lastLoopMillis = 0;

Master* master;
Button btnDifficulty0(1, INPUT_PULLUP);
Button btnDifficulty1(1, INPUT_PULLUP);
Button btnDifficulty2(1, INPUT_PULLUP);

void setup()
{
	Serial.println("Master booting");

	Wire.begin();
	Serial.begin(9600);

	// let modules boot first
	delay(100);

	master = new Master();

	// Development conveniences
	master->arm();
}


void loop()
{
	unsigned long now = millis();

	// Dont hammer the slaves, give them some breathing room
	if(now < lastLoopMillis + LOOP_INTERVAL_MS)
		return;

	// TODO: should we support ad hoc difficulty changing?
	if(btnDifficulty0.pressed())
		master->setDifficulty(0);
	else if(btnDifficulty1.pressed())
		master->setDifficulty(1);
	else if(btnDifficulty2.pressed())
		master->setDifficulty(2);

	master->loop(now);

	lastLoopMillis = now;
}

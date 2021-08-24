#include <Arduino.h>
#include <Wire.h>
#include <SD.h>
#include "master.cpp"

#include "../lib/button.h"

Master* master;
Button btnDifficulty0(7, INPUT_PULLUP);
Button btnDifficulty1(8, INPUT_PULLUP);
Button btnDifficulty2(9, INPUT_PULLUP);

void setup()
{
	Wire.setClock(100000);
	Wire.begin(4, 5);
	Serial.begin(9600);

	// let modules boot first
	delay(100);

	master = new Master();

	master->setDifficulty(1);
	master->arm();
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

	master->loop();
}

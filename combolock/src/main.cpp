#include <Arduino.h>
#include "combolock.h"

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

#ifdef UNIT_TEST

int main(int argc, char **argv) {
	return -1;
}

#endif
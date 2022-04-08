#include <Arduino.h>
#include "caeser.h"

Caeser* password;

void setup()
{
	Serial.begin(9600);
	
	password = new Caeser();
}

void loop()
{
	password->loop();
}

#ifdef UNIT_TEST

int main(int argc, char **argv) {
	return -1;
}

#endif
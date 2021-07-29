#include <Arduino.h>
#include "maze.cpp"

Maze maze;

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	maze.loop();
}
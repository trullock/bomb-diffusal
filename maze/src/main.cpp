#include <Arduino.h>
#include "maze.cpp"

Maze* maze;

void setup()
{
	Serial.begin(9600);
	maze = new Maze();
}

void loop()
{
	maze->loop();
}
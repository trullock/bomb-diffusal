#include <Arduino.h>

#define CLK_PIN   13  // or SCK
#define DATA_PIN  11  // or MOSI
#define CS_PIN    10  // or SS

#define I2CAddress 7
#define PIN_RaiseInterrupt 8
#define PIN_Buttons A2
#define PIN_Deactivated 5
#define PIN_RandomSeed A0

#include <maze.h>

Maze* maze;

void setup()
{
	Serial.begin(115200);
	maze = new Maze();
}

void loop()
{
	maze->loop();
}
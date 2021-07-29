#include <Wire.h>
#include <Arduino.h>
#include "mazeImpl.h"
#include <MD_MAX72xx.h>
#include <SPI.h>

#include "button.h"

#define I2CAddress 7

/// General Module details

#define Disarmed_LED_PIN 8

Maze maze(I2CAddress);

/// Specific Module details

//// Maze details

int posX = 0,
	 posY = 0;

int clue1X = 1, clue1Y = 4,
	clue2X = 5, clue2Y = 7;
int startX = 4, startY = 3;
#define rows 8
#define cols 8
bool maze[8+7][8+7] = {
  //     |     |     |     |     |     |     |
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

#define STATE_STRIKING 3

unsigned long nextMillis = 0;

Button btnNorth(1, INPUT);
Button btnEast(2, INPUT);
Button btnSouth(3, INPUT);
Button btnWest(4, INPUT);


void setup()
{
	Serial.begin(9600);
}

void loop()
{
	// if (!armed)
	// 	return;

	// if (deactivated)
	// 	return;

		handleMovement();
	else if(state == STATE_STRIKING)
	{
	if(!armed)
	}
	Maze.loop();
}


void handleMovement() {
	int newY = 0, newX = 0, mazeY = 0, mazeX = 0;

	if (btnNorth.pressed())
	{
		mazeY = posY - 1;
		mazeX = posX;
		newY = posY - 2;
		newX = posX;
	}
	else if (btnEast.pressed())
	{
		mazeY = posY;
		mazeX = posX + 1;
		newY = posY;
		newX = posX + 2;
	}
	else if (btnSouth.pressed())
	{
		mazeY = posY + 1;
		mazeX = posX;
		newY = posY + 2;
		newX = posX;
	}
	else if (btnWest.pressed())
	{
		mazeY = posY;
		mazeX = posX - 1;
		newY = posY;
		newX = posX - 2;
	}
	else
	{
		return;
	}

	if (mazeY < 0 || mazeY >= rows || mazeX < 0 || mazeX >= cols)
	{
		// oob crash
		strike();
	}
	else if (maze[mazeY][mazeX] == 1)
	{
		// wall crash
		strike();
	}
	else
	{
		posX = newX;
		posY = newY;
	}
}

void strike() {
	state = STATE_STRIKING;

	strikes++
}

void showStrike() {
	// todo: flash or something

	// go back to navigation afterwards
	state = STATE_NAVIGATING;
}


void reportStatus() {
	Wire.write(status);
	Wire.write(strikes);
	Wire.write(pendingNotification);
	pendingNotification = 0;
}

void arm()
{
	randomSeed(analogRead(0));

	armed = true;
}

void explode()
{
	// TODO:
}

void receiveCommand (int howMany)
{
	// Master scanning for modules will cause this
	if(howMany == 0)
		return;
	
	byte command = Wire.read();

	if(command == COMMAND_DIFFICULTY)
	{
		difficulty = Wire.read();
		
		Serial.print("Setting difficulty to: ");
		Serial.println(difficulty);

		if(armed)
			arm();
	}
	else if(command == COMMAND_ARM)
	{
		Serial.println("Arming");
		arm();
	}
	else if(command == COMMAND_EXPLODE)
	{
		explode();
	}
}
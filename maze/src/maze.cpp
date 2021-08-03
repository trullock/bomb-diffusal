#include <slave.h>
#include "../lib/button.h"
#include "../lib/MD_MAX72xx/MD_MAX72xx.h"

#define STATE_CLUE 1
#define STATE_NAVIGATING 2

class Maze : public Slave {

	// https://www.makerguides.com/max7219-led-dot-matrix-display-arduino-tutorial/
	// https://majicdesigns.github.io/MD_MAX72XX/class_m_d___m_a_x72_x_x.html
	MD_MAX72XX* display;

	Button* btnNorth;
	Button* btnEast;
	Button* btnSouth;
	Button* btnWest;

	/// Pulsing
	
	float pulsePhase = 0;
	unsigned long nextPulseMillis = 0;

	// Positioning
	int posX = 0,
		posY = 0,
		finishX = 0,
		finishY = 0;

	// Maze details
	int clue1X = 1, clue1Y = 4,
		clue2X = 5, clue2Y = 7;
	int startX = 4, startY = 3;
	#define rows 8
	#define cols 8

	// 0 = blank
	// 1 = wall
	// 2 = start
	// 3 = finish
	const byte maze[1][8 + 7][8 + 7] = {
		{
			//  |     |     |     |     |     |     |
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
			{2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
			{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
		}
	};

	void setDifficulty(byte diff) override 
	{
		Slave::setDifficulty(diff);

		randomSeed(millis());
		int r = random(0, 0);
		for(byte y = 0; y < rows + rows - 1; y++)
		for(byte x = 0; x < cols + cols - 1; x++)
		{
			if(maze[r][y][x] == 2)
			{
				posX = x;
				posY = y;
			}
			else if(maze[r][y][x] == 3)
			{
				finishX = x;
				finishY = y;
			}
		}
	}

	void arm() override
	{
		Slave::arm();

		this->state = STATE_CLUE;

		display->clear();
		display->setPoint(clue1X, clue1Y, true);
		display->setPoint(clue2X, clue2Y, true);
		display->update();
	}

	void pulseDisplay()
	{
		if (nextPulseMillis > millis())
			return;

		nextPulseMillis = millis() + 100;

		pulsePhase += 0.05;
		if (pulsePhase > 2)
			pulsePhase = 0;

		int clueIntensity = abs(sin(pulsePhase * 3.142)) * 15.0 / 2;

		display->control(MD_MAX72XX::controlRequest_t::INTENSITY, clueIntensity);
		display->update();
	}

	void handleStartNavigating()
	{
		if(this->btnNorth->pressed() || this->btnEast->pressed() || this->btnSouth->pressed() || this->btnWest->pressed())
			this->state = STATE_NAVIGATING;
	}

	void handleMovement()
	{
		int newY = 0, newX = 0, mazeY = 0, mazeX = 0;

		if (this->btnNorth->pressed())
		{
			mazeY = this->posY - 1;
			mazeX = this->posX;
			newY = this->posY - 2;
			newX = this->posX;
		}
		else if (this->btnEast->pressed())
		{
			mazeY = this->posY;
			mazeX = this->posX + 1;
			newY = this->posY;
			newX = this->posX + 2;
		}
		else if (this->btnSouth->pressed())
		{
			mazeY = this->posY + 1;
			mazeX = this->posX;
			newY = this->posY + 2;
			newX = this->posX;
		}
		else if (this->btnWest->pressed())
		{
			mazeY = this->posY;
			mazeX = this->posX - 1;
			newY = this->posY;
			newX = this->posX - 2;
		}
		else
		{
			return;
		}

		if (mazeY < 0 || mazeY >= rows || mazeX < 0 || mazeX >= cols)
		{
			// TODO: support wrap around?
			// oob crash
			reportStrike();
		}
		else if (maze[0][mazeY][mazeX] == 1)
		{
			// wall crash
			reportStrike();
		}
		else
		{
			posX = newX;
			posY = newY;

			display->clear();
			display->setPoint(posX, posY, true);
			display->update();

			if(posX == finishX && posY == finishY)
			{
				this->state = STATE_DEACTIVATED;
			}
		}
	}

	void strike() override {
		Slave::strike();

		for (int i = 0; i < rows; i++)
			display->setRow(i, 1);
		display->update();
	}

	void explode() override
	{
		Slave::explode();

		for (int i = 0; i < rows; i++)
			display->setRow(i, 1);
		display->update();
	}

	void deactivate() override {
		Slave::deactivate();

		this->display->clear();
		this->display->update();
	}

public:
	Maze() : Slave(7)
	{
		this->display = new MD_MAX72XX(MD_MAX72XX::FC16_HW, 3, 4);

		this->btnNorth = new Button(1, INPUT);
		this->btnEast = new Button(2, INPUT);
		this->btnSouth = new Button(3, INPUT);
		this->btnWest = new Button(4, INPUT);

		this->display->begin();

		this->display->control(MD_MAX72XX::controlRequest_t::INTENSITY, 15);
		this->display->clear();
		this->display->update();
	}

	void loop()
	{
		Slave::loop();

		if (state == STATE_CLUE || state == STATE_STRIKING || state == STATE_EXPLODING)
		{
			this->pulseDisplay();
			this->handleStartNavigating();
		}
		else if(state == STATE_NAVIGATING)
		{
			this->handleMovement();
		}
	}
};
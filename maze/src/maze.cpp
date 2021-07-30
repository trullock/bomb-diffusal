#include "../lib/slave/slave.h"
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
		posY = 0;

	// Maze details
	int clue1X = 1, clue1Y = 4,
		clue2X = 5, clue2Y = 7;
	int startX = 4, startY = 3;
	#define rows 8
	#define cols 8
	bool maze[8 + 7][8 + 7] = {
		//     |     |     |     |     |     |     |
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // -
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	};

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
			// oob crash
			reportStrike();
		}
		else if (maze[mazeY][mazeX] == 1)
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

	void loop() {

		if (state == STATE_CLUE || state == STATE_STRIKING || state == STATE_EXPLODING)
		{
			this->pulseDisplay();
		}
		else if(state == STATE_NAVIGATING)
		{
			this->handleMovement();
		}
	}
};
#include <slave.h>
#include <AnalogButtons.h>
#include <MD_MAX72xx.h>

#define STATE_CLUE 1
#define STATE_NAVIGATING 2


class Maze : public Slave {

	// https://www.makerguides.com/max7219-led-dot-matrix-display-arduino-tutorial/
	// https://majicdesigns.github.io/MD_MAX72XX/class_m_d___m_a_x72_x_x.html
	MD_MAX72XX* display;
	AnalogButtons* buttons;

	/// Display animations
	float pulsePhase = 0;
	unsigned long nextPulseMillis = 0;
	unsigned long nextMillis = 0;
	uint8_t deactivationAnimationStage = 0;

	// Positioning
	int posX = 0,
		posY = 0,
		finishX = 0,
		finishY = 0,
		mazeIndex = 0;

	#define rows 8
	#define cols 8

	// Mazes 
	#define CLUE1X 0
	#define CLUE1Y 1
	#define CLUE2X 2
	#define CLUE2Y 3
	#define STARTX 4
	#define STARTY 5
	#define FINISHX 6
	#define FINISHY 7
	const uint8_t mazeFeatures[1][8] = {
		1, 4, // clue 1 x,y
		5, 6, // clue 2 x,y
		0, 2, // start x,y
		6, 6 // finish x,y
	};
	const uint8_t mazes[1][2][8] = {
		{
			// Walls in X direction (by row)
			{
				0b0110000,
				0b1101010,
				0b1011010,
				0b1011111,
				0b0111100,
				0b1011100,
				0b1011111,
				0b0010110
			},
			// Walls in Y direction (by col)
			{
				0b0000000,
				0b0010101,
				0b0010010,
				0b1000000,
				0b0100000,
				0b1001000,
				0b1010110,
				0b0100100
			}
		}
	};

	void setDifficulty(byte diff) override 
	{
		Slave::setDifficulty(diff);

		randomSeed(analogRead(PIN_RandomSeed));
		
		uint8_t min = diff * 3;
		uint8_t max = min + 3;

		// TODO: Keep in sync with number of mazes
		min = 0;
		max = 1;

		int r = random(min, max);
		
		this->mazeIndex = r;
		
		debug();
	}

	void debug()
	{
		Serial.print(' ');
		for(char w = 0; w < (cols * 2) - 1; w++)
			Serial.print('_');
		Serial.println(' ');

		for(char y = 0; y < rows; y++)
		{
			Serial.print('|');
			for(char w = 0; w < cols; w++)
			{
				if(y < 7)
					Serial.print((1 << (6-y)) & mazes[this->mazeIndex][1][w] ? '_' : ' ');
				else
					Serial.print('_');
					
				if(w < 7)
					Serial.print((1 << (6-w)) & mazes[this->mazeIndex][0][y] ? '|' : y < 7 ? ' ' : '_');
			}
			Serial.print('|');
			Serial.println();
		}
	}

	void reset()
	{
		this->deactivationAnimationStage = 0;
	}

	void arm() override
	{
		this->reset();
		Slave::arm();
		this->showClue();
	}

	void showClue()
	{
		this->state = STATE_CLUE;
		display->clear();
		display->setPoint(this->mazeFeatures[this->mazeIndex][CLUE1X], this->mazeFeatures[this->mazeIndex][CLUE1Y], true);
		display->setPoint(this->mazeFeatures[this->mazeIndex][CLUE2X], this->mazeFeatures[this->mazeIndex][CLUE2Y], true);
		display->transform(MD_MAX72XX::transformType_t::TRC);
		display->update();
	}

	void pulseDisplay()
	{
		if (nextPulseMillis > millis())
			return;

		nextPulseMillis = millis() + 50;

		pulsePhase += 20;
		if (pulsePhase >= 360)
			pulsePhase = 0;
		
		#define PI 3.14159265
		int clueIntensity = (sin(pulsePhase * PI/180) + 1) * 8.0;

		display->control(MD_MAX72XX::controlRequest_t::INTENSITY, clueIntensity);
		display->update();
	}

	void handleStartNavigating()
	{
		uint8_t btn = this->buttons->readButton();

		if(btn != BTN_NONE)
		{
			Serial.println("Started navigation");

			this->posX = this->mazeFeatures[this->mazeIndex][STARTX];
			this->posY = this->mazeFeatures[this->mazeIndex][STARTY];

			this->state = STATE_NAVIGATING;
			this->updateNavigatingDisplay();
		}
	}

	void handleMovement()
	{
		int newPosY = 0, newPosX = 0;
		
		int8_t btn = this->buttons->readButton();

		if(btn == BTN_NONE)
			return;

		uint8_t wallCollision = 0;

		if (btn == BTN_UP)
		{
			newPosY = this->posY - 1;
			newPosX = this->posX;
			wallCollision = (1 << (7 - posY)) & mazes[this->mazeIndex][1][posX];
		}
		else if (btn == BTN_RIGHT)
		{
			newPosY = this->posY;
			newPosX = this->posX + 1;
			wallCollision = (1 << (7 - newPosX)) & mazes[this->mazeIndex][0][posY];
		}
		else if (btn == BTN_DOWN)
		{
			newPosY = this->posY + 1;
			newPosX = this->posX;
			wallCollision = (1 << (7 - newPosY)) & mazes[this->mazeIndex][1][posX];
		}
		else if (btn == BTN_LEFT)
		{
			newPosY = this->posY;
			newPosX = this->posX - 1;
			wallCollision = (1 << (7 - posX)) & mazes[this->mazeIndex][0][posY];
		}
		else if(btn == BTN_ENTER)
		{
			if(posX == this->mazeFeatures[this->mazeIndex][FINISHX] && posY == this->mazeFeatures[this->mazeIndex][FINISHY])
			{
				this->deactivate();
				return;
			}

			this->reportStrike();
		}

		if (wallCollision != 0)
		{
			Serial.println("Wall collision");
			reportStrike();
			return;
		}

		if (newPosY < 0 || newPosY == rows || newPosX < 0 || newPosY == cols)
		{
			// TODO: support wrap around?
			// oob crash
			Serial.println("Out of bounds");
			reportStrike();
			return;
		}

		posX = newPosX;
		posY = newPosY;

		this->updateNavigatingDisplay();
	}

	void updateNavigatingDisplay()
	{
		display->clear();
		display->setPoint(posX, posY, true);
		display->control(MD_MAX72XX::controlRequest_t::INTENSITY, MAX_INTENSITY);
		display->transform(MD_MAX72XX::transformType_t::TRC);
		display->update();	
	}

	void strike() override
	{
		Slave::strike();

		this->nextMillis = millis() + STRIKE_DURATION_MS;

		for (int y = 0; y < rows; y++)
		for (int x = 0; x < cols; x++)
			// TODO: set buffer instead to be faster
			display->setPoint(y, x, 1);
		display->update();
	}

	void stopStriking()
	{
		this->showClue();
	}

	void explode() override
	{
		Slave::explode();

		for (int i = 0; i < rows; i++)
			display->setRow(i, 1);
		display->update();
	}

	void deactivationAnimation()
	{
		if (nextPulseMillis > millis())
			return;

		nextPulseMillis = millis() + 50;

		uint8_t border = deactivationAnimationStage;

		this->display->clear();

		if(this->deactivationAnimationStage == 6)
			return;

		for(uint8_t y = border; y < rows - border; y++)
		{
			for(uint8_t x = border; x < cols - border; x++)
			{
				if(y == border || y == rows - border - 1
					|| x == border || x == cols - border - 1)
				{
					this->display->setPoint(y, x, true);
				}
			}	
		}
		this->display->transform(MD_MAX72XX::transformType_t::TRC);
		this->display->update();

		this->deactivationAnimationStage++;

	}

public:
	Maze() : 
		Slave(I2CAddress, PIN_RaiseInterrupt, PIN_Deactivated)
	{
		this->display = new MD_MAX72XX(MD_MAX72XX::FC16_HW, 10, 13, 11, 1);
		this->display->begin();
		this->display->control(MD_MAX72XX::controlRequest_t::INTENSITY, 15);
		this->display->clear();
		this->display->update();

		this->buttons = new AnalogButtons(PIN_Buttons);

		#ifdef DEVMODE
			this->setDifficulty(1);
			this->arm();
		#endif
	}

	void loop()
	{
		Slave::loop();

		if (state == STATE_CLUE)
		{
			this->pulseDisplay();
			this->handleStartNavigating();
		}
		else if(state == STATE_NAVIGATING)
		{
			this->handleMovement();
		}
		else if(this->state == STATE_STRIKING)
		{
			this->pulseDisplay();
			if(millis() >= nextMillis)
				this->stopStriking();
		}
		else if(this->state == STATE_EXPLODING)
		{
			this->pulseDisplay();
		}
		else if(this->state == STATE_DEACTIVATED)
		{
			this->deactivationAnimation();
		}
	}
};
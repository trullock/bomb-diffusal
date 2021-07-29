#include "../common/Slave.cpp"
#include "./MD_MAX72xx/MD_MAX72xx.h"
#include "../common/button.h"

#define STATE_CLUE 1
#define STATE_NAVIGATING 2

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 3


#define PulseGapMillis 100

class Maze : public Slave {

	int state = 0;

	// https://www.makerguides.com/max7219-led-dot-matrix-display-arduino-tutorial/
	// https://majicdesigns.github.io/MD_MAX72XX/class_m_d___m_a_x72_x_x.html
	MD_MAX72XX display;//(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

	Button btnNorth(1, INPUT);
	Button btnEast(2, INPUT);
	Button btnSouth(3, INPUT);
	Button btnWest(4, INPUT);

	/// Clue pulsing
	float cluePhase = 0;
	unsigned long nextPulseMillis = 0;

public:
	Maze(int i2cAddress) : Slave(i2cAddress)
	{
		this->display.begin();
		this->display.control(2, 1); // intensity
		this->display.clear();
		this->display.update();

		this->state = STATE_CLUE;
	}

	void loop() {
		if(state == STATE_CLUE)
		{
			pulseClue();
		}
		else if(state == STATE_NAVIGATING)
		{
			showLocation();
		}

		if(!this->armed)
			return;

		if(this->deactivated)
			return;
	}
		
	void showClue() {
		display.clear();
		display.setPoint(clue1X, clue1Y, true);
		display.setPoint(clue2X, clue2Y, true);
		display.update();
	}

	void showLocation() {
		display.clear();
		display.setPoint(posX, posY);
		display.update();
	}

	void pulseClue() {
		if(nextPulseMillis > millis())
			return;

		nextPulseMillis = millis() + PulseGapMillis;

		cluePhase += 0.05;
		if(cluePhase > 2)
			cluePhase = 0;

		int clueIntensity = abs(sin(cluePhase * 3.142)) * 15.0/2;

		display.control(2, clueIntensity);
		display.update();
	}

};
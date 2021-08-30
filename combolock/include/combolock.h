#include <slave.h>
#include <Encoder.h>
#include <button.h>
#include <LedControl.h>
#include <serialnumber.h>
#include <lock.h>

#define DIRECTION_NONE 0
#define DIRECTION_LEFT 1
#define DIRECTION_RIGHT 2
#define DIRECTION_ANY 3

#define STATE_TURNING 1

class Combolock : public Slave {

	unsigned long nextMillis;

	/// Button details
	Button btnEnter;

	//// Knob details
	Encoder knob;
	int32_t knobPosition = 0;
	
	byte lockPositionIndex = 0;
	uint8_t lockPosition = 0;
	uint8_t correctDirection = DIRECTION_LEFT;
	
	byte combination[5];

	// Display
	LedControl display;

	void setSerialNumber(byte a, byte b, byte c, byte d, byte e) override
	{
		Slave::setSerialNumber(a, b, c, d, e);

		setCombination(this->combination, this->serialNumber);
	}

	void arm()
	{
		Slave::arm();

		this->state = STATE_TURNING;

		this->lockPosition = 0;
		this->lockPositionIndex = 0;
		this->correctDirection = DIRECTION_ANY;
	}

	void strike() override {
		Slave::strike();

		this->nextMillis = millis() + STRIKE_DURATION_MS;

		display.setDigit(0, 0, 8, true);
		display.setDigit(0, 1, 8, true);
		display.setDigit(0, 2, 8, true);
		display.setDigit(0, 3, 8, true);
	}

	void explode() override
	{
		Slave::explode();
	}

	void deactivate() override
	{
		Slave::deactivate();

		display.setIntensity(0, 1);
		display.setChar(0, 0, '-', false);
		display.setChar(0, 1, '-', false);
		display.setChar(0, 2, '-', false);
		display.setChar(0, 3, '-', false);
	}

	void handleKnob()
	{
		int32_t position = knob.read();
		if (position != knobPosition)
		{
			uint8_t direction = position < knobPosition ? DIRECTION_LEFT : DIRECTION_RIGHT;
			knobPosition = position;
			lockPosition += position;

			// TODO can the encoder ever skip numbers? Shouldnt think so

			this->handleLockPositionChange(direction);
		}
	}

	void handleLockPositionChange(uint8_t direction)
	{
		// If we turned the knob the right way
		if(direction == this->correctDirection || this->correctDirection == DIRECTION_ANY)
		{
			// if we've reached the correct position
			if(this->lockPosition == this->combination[this->lockPositionIndex])
			{
				// if we've solved all the positions we need
				if(this->lockPositionIndex == 2 + this->difficulty)
				{
					this->correctDirection = DIRECTION_NONE;
					return;
				}

				// reverse the direction for the next number
				this->correctDirection = direction == DIRECTION_LEFT
											? DIRECTION_RIGHT 
											: DIRECTION_LEFT;
				this->lockPositionIndex++;
			}
			return;
		}
	
		// else we've gone wrong, start again
		this->lockPositionIndex = 0;
		this->correctDirection = DIRECTION_ANY;
	}

	void handleButton()
	{
		if (!this->btnEnter.pressed())
			return;

		// if we dont need to do any more turning
		if (this->correctDirection == DIRECTION_NONE)
		{
			this->deactivate();
			return;
		}

		this->reportStrike();
	}

	void updateDisplay()
	{
		int hundreds = this->lockPosition / 100;
		int tens = (this->lockPosition - hundreds * 100) / 10;
		int ones = this->lockPosition % 10;

		display.setDigit(0, 0, ones, false);
		display.setDigit(0, 1, tens, false);
		display.setDigit(0, 2, hundreds, false);
	}

	void stopStriking()
	{
		this->state = STATE_TURNING;
		display.clearDisplay(0);
	}

public:
	Combolock() : 
		Slave(12, 2), 
		btnEnter(8, INPUT_PULLUP), 
		knob(6, 7),
		display(12, 11, 10, 1)
	{
		this->display.shutdown(0, false);
		this->display.setIntensity(0, 8);
		this->display.clearDisplay(0);
	}

	void loop()
	{
		Slave::loop();

		if(this->state == STATE_TURNING)
		{
			this->updateDisplay();

			this->handleButton();
			this->handleKnob();
		}
		else if(this->state == STATE_STRIKING)
		{
			if(millis() >= nextMillis)
				this->stopStriking();
		}
	}
};
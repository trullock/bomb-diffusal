#include <slave.h>
#include <RotaryEncoder.h>
#include <button.h>
#include <serialnumber.h>
#include <lock.h>
#include <whmxe_595.h>

#ifndef Combolock_h
#define Combolock_h

/* ----------- Pins ----------- */
#define PIN_Deactivated 9
#define PIN_MasterInterrupt A0
#define PIN_RotaryA 5
#define PIN_RotaryB 6
#define PIN_Button 10
#define PIN_Load A1
#define PIN_Sclk A2
#define PIN_SDI A3



#define DIRECTION_NONE 0
#define DIRECTION_LEFT 1
#define DIRECTION_RIGHT 2
#define DIRECTION_ANY 3

#define STATE_TURNING 1


class Combolock : public Slave {

	unsigned long nextMillis;
	unsigned long nextFlashMillis = 0;
	bool nextFlashState = 0;

	/// Button details
	Button btnEnter;

	//// Knob details
	RotaryEncoder knob;
	int32_t knobPosition = 0;
	
	byte lockPositionIndex = 0;
	uint8_t lockPosition = 0;
	uint8_t correctDirection = DIRECTION_LEFT;
	
	byte combination[5];

	// Display
	whmxe_595 display;

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
		this->nextFlashMillis = 0;
		flashStrike();
	}

	void explode() override
	{
		Slave::explode();
	}

	void deactivate() override
	{
		Slave::deactivate();

		display.clear();
	}

	void handleKnob()
	{
		knob.tick();
		int32_t position = knob.getPosition();

		if (position != knobPosition)
		{
			// TODO: handle overflow loops? int32 is pretty big, can probably ignore
			
			uint8_t direction = position < knobPosition ? DIRECTION_LEFT : DIRECTION_RIGHT;
			knobPosition = position;
			lockPosition += direction == DIRECTION_LEFT ? -1 : 1;

		
			// TODO: what should happen below, does this effect direction-correctness?

			// restrict to 0-99 range
			if(lockPosition == 100)
				lockPosition = 0;
			else if(lockPosition == 255)
				lockPosition = 99;

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

		Serial.println("Invalid combination");
		Serial.flush();
		this->reportStrike();
	}

	void updateDisplay()
	{
		display.setNumber(this->lockPosition);
	}

	void flashStrike()
	{
		if(this->nextFlashMillis > millis())
			return;

		if(this->nextFlashState)
		{
			display.setNumber(88);
			this->setDeactivatedLED(true);
		}
		else
		{
			display.clear();
			this->setDeactivatedLED(false);
		}

		this->nextFlashState = !this->nextFlashState;

		this->nextFlashMillis += 200;
	}

	void stopStriking()
	{
		this->state = STATE_TURNING;
		this->updateDisplay();
	}

public:
	Combolock() : 
		Slave(2, PIN_MasterInterrupt, PIN_Deactivated), 
		btnEnter(PIN_Button, INPUT_PULLUP),
		knob(PIN_RotaryA, PIN_RotaryB, RotaryEncoder::LatchMode::FOUR3),
		display(PIN_SDI, PIN_Sclk, PIN_Load)
	{
		this->display.clear();

		#ifdef DEVMODE
			this->setSerialNumber(12, 26, 2, 17, 1);
			this->setDifficulty(1);
			this->arm();
		#endif
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
			else
			this->flashStrike();
		}
	}
};

#endif
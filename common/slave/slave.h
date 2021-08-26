#include <Arduino.h>
#include <Wire.h>
#include "../constants.h"

class Slave {

	// These three are hacks to hook into Wire's I2C implementation
	static Slave* self;
	static void reportStatusWrapper();
	static void receiveCommandWrapper(int x);

	/**
	 * Reports the status of this module back to the master
	 */
	void reportStatus();

	/**
	 * Receives commands from the master and stores them in the temporary commandBuffer
	 */
	void receiveCommand(int howMany);

	/**
	 * Turns the master-interrupt off
	 */
	void endMasterInterrupt();

	volatile byte commandBuffer[12];
	volatile byte commandBufferLength = 0;
	
	unsigned int raiseInterruptPin;
	bool interrupting;

	volatile byte sfxQueue[MODULE_MAX_SFX_QUEUE_LENGTH];
	volatile byte sfxQueueLength = 0;

protected:

	byte strikes = 0;
	byte state = STATE_DISARMED;
	byte difficulty = 1;
	uint16_t timeRemainingInSecs = 0;
	byte serialNumber[5];

	/**
	 * Handles commands from the master.
	 * This is called when the master sends us a command.
	 */
	void handleCommand();

	/**
	 * Instructs the master to query us for new status information
	 */
	void raiseMasterInterrupt();

	/**
	 * Update's this module's awareness of how long remains.
	 * This is called by the master.
	 * This exists instead of an internal timer as the master can arbitrarily decide the time remaining at any point
	 */
	virtual void updateTimeRemaining(uint16_t secs);

	/**
	 * Sets the system's Serial Number
	 */
	void setSerialNumber(byte a, byte b, byte c, byte d, byte e);

	/**
	 * Sets this module's difficulty level.
	 * This is called by the master
	 */
	virtual void setDifficulty(byte difficulty);

	/**
	 * Arms this module, ready for interaction by the user.
	 * This is called when the master tells us we're armed.
	 */
	virtual void arm();

	/**
	 * Causes this module to display a strike.
	 * This is called when the master tells us a strike has occurred on any module, not just this one.
	 */
	virtual void strike();

	/**
	 * Causes this module to explode.
	 * This is called when the master tells us we've exploded.
	 */
	virtual void explode();

	/**
	 * Deactivates this module.
	 * Should be call ones the module has been successfully deactivated.
	 */
	virtual void deactivate();

	/**
	 * Reports a strike to the master.
	 * Should be called when this module has caused a Strike.
	 */
	virtual void reportStrike();

	/**
	 * Queues a sound effect to be sent to the master.
	 * The sfx will not be sent until the next call to raiseMasterInterrupt();
	 */
	virtual bool queueSfx(byte sound);

public:

	Slave(byte i2cAddress);

	/**
	 * Creates a new slave on the given I2C address (1-254) using the given master-interrupt pin
	 */
	Slave(byte i2cAddress, uint8_t raiseInterruptPin);

	/**
	 * Must be called regularly to enable this module to perform its duties
	 */
	virtual void loop();
};
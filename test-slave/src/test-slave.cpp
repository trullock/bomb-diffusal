#include <Slave.h>
#include <button.h>
#include <sounds.h>
 
class TestSlave : public Slave {

public:

	Button btnStrike;
	Button btnDeactivate;
	Button btnNotify;

	TestSlave() :
		Slave(1, 12),
		btnStrike(8, INPUT_PULLUP),
		btnDeactivate(9, INPUT_PULLUP),
		btnNotify(10, INPUT_PULLUP)
	{
		
	}

	void updateTimeRemaining(uint16_t secs) override
	{
		Slave::updateTimeRemaining(secs);

		Serial.print("Time remaining: ");
		Serial.print(secs);
		Serial.println("s");
	}

	void loop()
	{
		Slave::loop();

		if(btnStrike.pressed())
			this->reportStrike();

		if(btnDeactivate.pressed())
			this->deactivate();
		
		if(btnNotify.pressed())
		{
			this->queueSfx(Sounds::SatelliteUplinkEstablished);
			this->raiseMasterInterrupt();
		}
	}
};
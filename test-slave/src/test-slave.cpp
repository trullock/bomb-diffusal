#include <Slave.h>
#include <button.h>
#include <sounds.h>
 
class TestSlave : public Slave {

public:

	Button btnStrike;
	Button btnDeactivate;
	Button btnNotify;

	TestSlave() :
		Slave(1, 2),
		btnStrike(8, INPUT_PULLUP),
		btnDeactivate(9, INPUT_PULLUP),
		btnNotify(10, INPUT_PULLUP)
	{
		
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
			this->queueSfx(Sounds::IncomingMessage);
			this->queueSfx(Sounds::NatoAlpha);
			this->queueSfx(Sounds::NatoBravo);
			this->queueSfx(Sounds::NatoCharlie);
			this->queueSfx(Sounds::NatoDelta);
			this->raiseMasterInterrupt();
		}
	}
};
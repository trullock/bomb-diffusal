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
			this->playSfx(Sounds::IncomingMessage);
			this->playSfx(Sounds::NatoAlpha);
			this->playSfx(Sounds::NatoBravo);
			this->playSfx(Sounds::NatoCharlie);
			this->playSfx(Sounds::NatoDelta);
				this->raiseMasterInterrupt();

		}
	}
};
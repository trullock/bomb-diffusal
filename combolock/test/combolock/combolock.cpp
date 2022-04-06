#include <ArduinoFake.h>
#include <unity.h>

using namespace fakeit;

#include <MasterFake.h>
#include <Wire.h>

#include <lock.h>
#include <serialnumber.h>
#include <combolock.h>
#include <Encoder.h>
#include "../include/arduino_test_setup.h"


void openLock(char a, char b, char c, char d, char e, int m1, int m2, int m3, int m4, int m5)
{
	Combolock lock;

	Master_SendCommand_SetSerialNumber(a, b, c, d, e);
	Master_SendCommand_SetDifficulty(2);
	Master_SendCommand_Arm();

	lock.loop();

	for(uint8_t i = 1; i <= abs(m1); i++)
	{
		Encoder::Encoder0->write(m1 > 0 ? i : i * -1);
		lock.loop();
	}
	
	for(uint8_t i = 1; i <= abs(m2); i++)
	{
		Encoder::Encoder0->write(m1 > 0 ? i : i * -1);
		lock.loop();
	}
	
	for(uint8_t i = 1; i <= abs(m3); i++)
	{
		Encoder::Encoder0->write(m1 > 0 ? i : i * -1);
		lock.loop();
	}
	
	for(uint8_t i = 1; i <= abs(m4); i++)
	{
		Encoder::Encoder0->write(m1 > 0 ? i : i * -1);
		lock.loop();
	}
	
	for(uint8_t i = 1; i <= abs(m5); i++)
	{
		Encoder::Encoder0->write(m1 > 0 ? i : i * -1);
		lock.loop();
	}

	Master_Verify_DidReceiveDeactivation(2, 0);
}

void playComboGames()
{
	openLock('A', 'B', '1', 'F', '3', 0, 0, 0, 0, 0);
}

void setUp(void)
{
    ArduinoFakeReset();

	MockFunctions();
	MockSerial();
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(playComboGames);

    UNITY_END();

    return 0;
}
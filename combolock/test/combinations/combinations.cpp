#include <ArduinoFake.h>
#include <unity.h>

using namespace fakeit;

#include <lock.h>
#include <combolock.h>
#include <serialnumber.h>
#include "../include/arduino_test_setup.h"


void ensure_combinations_meet_requirements(void) {
	
	byte combination[5];
	byte serialNumber[5];

	srand(time(NULL));

	for(uint16_t i = 0; i < 10; i++)
	{
		generateSerialNumber(serialNumber);

		setCombination(combination, serialNumber);

		printf("SerialNumber: %c%c%c%c%c, Combinations: %d %d %d %d %d\n", serialCharToAscii(serialNumber[0]), serialCharToAscii(serialNumber[1]), serialCharToAscii(serialNumber[2]), serialCharToAscii(serialNumber[3]), serialCharToAscii(serialNumber[4]), combination[0], combination[1], combination[2], combination[3], combination[4]);

		if(combination[0] == combination[1] 
			|| combination[1] == combination[2] 
			|| combination[2] == combination[3] 
			|| combination[3] == combination[4])
		{
			TEST_FAIL_MESSAGE("Consecutive combinations positions are the same");
		}

		if(combination[0] == 0)
		{
			String message = "First combination position is zero. S/N: ";
			message += serialCharToAscii(serialNumber[0]);
			message += serialCharToAscii(serialNumber[1]);
			message += serialCharToAscii(serialNumber[2]);
			message += serialCharToAscii(serialNumber[3]);
			message += serialCharToAscii(serialNumber[4]);

			TEST_FAIL_MESSAGE(message.c_str());
		}
	}

	TEST_PASS_MESSAGE("Combination generated cleanly");
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

    RUN_TEST(ensure_combinations_meet_requirements);

    UNITY_END();

    return 0;
}
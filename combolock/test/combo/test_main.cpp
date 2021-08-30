#include <ArduinoFake.h>
#include <unity.h>

#include <MasterFake.h>
#include <Wire.h>

#include <lock.h>
#include <serialnumber.h>
#include <combolock.h>

using namespace fakeit;

void ensure_no_consecutive_lock_combination_positions(void) {
	
	byte combination[5];
	byte serialNumber[5];

	for(uint16_t i = 0; i < 100000; i++)
	{
		srand(time(NULL));

		serialNumber[0] = rand() % 36;
		serialNumber[1] = rand() % 36;
		serialNumber[2] = rand() % 36;
		serialNumber[3] = rand() % 36;
		serialNumber[4] = rand() % 36;

		setCombination(combination, serialNumber);

		if(combination[0] == combination[1] 
			|| combination[1] == combination[2] 
			|| combination[2] == combination[3] 
			|| combination[3] == combination[4])
		{
			TEST_FAIL_MESSAGE("Consecutive combinations positions are the same");
		}

		TEST_PASS_MESSAGE("No consecutive combintions positions the same");
	}
}


void setUp(void)
{
    ArduinoFakeReset();
}

void lol()
{
    When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
	
	When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
	When(Method(ArduinoFake(), shiftOut)).AlwaysReturn();
	When(Method(ArduinoFake(), digitalRead)).AlwaysReturn(LOW);

	When(Method(ArduinoFake(), cli)).AlwaysReturn();
	When(Method(ArduinoFake(), sei)).AlwaysReturn();

	When(Method(ArduinoFake(), micros)).AlwaysReturn(100000);
	When(Method(ArduinoFake(), millis)).AlwaysReturn(200000);
	
	When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const String &))).AlwaysReturn();
	When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const String &))).AlwaysReturn();

	When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const char *))).AlwaysReturn();
	When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char *))).AlwaysReturn();

	
	When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const char[]))).AlwaysReturn();
	When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char[]))).AlwaysReturn();
	
	When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char))).AlwaysReturn();
	When(OverloadedMethod(ArduinoFake(Serial), println, size_t(char))).AlwaysReturn();

	When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned char, int))).AlwaysReturn();
	When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned char, int))).AlwaysReturn();
	
	When(OverloadedMethod(ArduinoFake(Serial), print, size_t(int, int))).AlwaysReturn();
	When(OverloadedMethod(ArduinoFake(Serial), println, size_t(int, int))).AlwaysReturn();

	When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned int, int))).AlwaysReturn();
	When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned int, int))).AlwaysReturn();

	When(OverloadedMethod(ArduinoFake(Serial), print, size_t(long, int))).AlwaysReturn();
	When(OverloadedMethod(ArduinoFake(Serial), println, size_t(long, int))).AlwaysReturn();
	
	When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned long, int))).AlwaysReturn();
	When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned long, int))).AlwaysReturn();

	When(OverloadedMethod(ArduinoFake(Serial), print, size_t(double, int))).AlwaysReturn();
	When(OverloadedMethod(ArduinoFake(Serial), println, size_t(double, int))).AlwaysReturn();

	When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const Printable&))).AlwaysReturn();
	When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const Printable&))).AlwaysReturn();

	When(OverloadedMethod(ArduinoFake(Serial), println, size_t(void))).AlwaysReturn();


	Combolock lock;

	Master_SendCommand_SetDifficulty(1);
	Master_SendCommand_Arm();

	lock.loop();
}


int main(int argc, char **argv) {
	UNITY_BEGIN();

	RUN_TEST(ensure_no_consecutive_lock_combination_positions);
	RUN_TEST(lol);

	UNITY_END();

	return 0;
}
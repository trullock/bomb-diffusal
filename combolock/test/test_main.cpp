#include <unity.h>
#include <ArduinoFake.h>

#include <lock.h>
#include <serialnumber.h>

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


int main(int argc, char **argv) {
	UNITY_BEGIN();

	RUN_TEST(ensure_no_consecutive_lock_combination_positions);

	UNITY_END();

	return 0;
}
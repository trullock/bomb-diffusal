#ifndef MasterFake_h
#define MasterFake_h

#include <constants.h>
#include <stdint.h>
#include <Wire.h>


void Master_SendCommand_Arm()
{
	Wire.test_Receive(COMMAND_ARM);
}

void Master_SendCommand_SetDifficulty(uint8_t diff)
{
	Wire.test_Receive(COMMAND_DIFFICULTY, diff);
}

#endif
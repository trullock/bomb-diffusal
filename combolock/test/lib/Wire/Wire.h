#ifndef Wire_h
#define Wire_h

#include <stdint.h>

class Wire_
{
	uint8_t buffer[8];
	uint8_t bufferLength = 0;
	uint8_t readIndex = 0;

    static void (*user_onRequest)(void);
    static void (*user_onReceive)(int);

	public:

	bool begin(uint8_t address);
	void onReceive( void (*function)(int) );
	void onRequest( void (*function)(void) );
	uint8_t read();
	void write(uint8_t b);

	void test_Receive(uint8_t arg0, uint8_t arg1 = 255, uint8_t arg2 = 255, uint8_t arg3 = 255, uint8_t arg4 = 255);
};


extern Wire_ Wire;
extern uint8_t TWAR;

#endif
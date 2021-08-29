#ifndef Wire_h
#define Wire_h

#include <stdint.h>

class Wire_
{
	public:
	bool begin(uint8_t address);
	void onReceive( void (*function)(int) );
	void onRequest( void (*function)(void) );
	uint8_t read();
	void write(uint8_t b);
};


extern Wire_ Wire;
extern uint8_t TWAR;

#endif
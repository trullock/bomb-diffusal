
#include "Wire.h"

bool Wire_::begin(uint8_t address) { return true; }

void Wire_::onReceive( void (*function)(int) ) {}

void Wire_::onRequest( void (*function)(void) ) {}

uint8_t Wire_::read(){ return 0; }
void Wire_::write(uint8_t b) { }


Wire_ Wire;

// register simulation
uint8_t TWAR = 0;

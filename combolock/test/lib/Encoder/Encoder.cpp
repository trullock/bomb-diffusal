#include "Encoder.h"

Encoder* Encoder::Encoder0;

Encoder::Encoder(uint8_t pinA, uint8_t pinB)
{
	this->position = 0;

	if(!Encoder0)
		Encoder0 = this;
}

int32_t Encoder::read()
{
	return this->position;
}

void Encoder::write(int32_t pos)
{
	this->position = pos;
}

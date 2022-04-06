#ifndef Encoder_h
#define Encoder_h

#include <stdint.h>

	class Encoder
	{
		int32_t position;

		public:

		static Encoder* Encoder0;

		Encoder(uint8_t pinA, uint8_t pinB);

		int32_t read();

		void write(int32_t pos);
	};

#endif
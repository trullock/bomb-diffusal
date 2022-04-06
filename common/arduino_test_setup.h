#ifndef Arduino_Test_Setup_h
#define Arduino_Test_Setup_h

#include <ArduinoFake.h>

	void MockSerial()
	{
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
	}

	void MockFunctions()
	{
		When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
		
		When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
		When(Method(ArduinoFake(), shiftOut)).AlwaysReturn();
		When(Method(ArduinoFake(), digitalRead)).AlwaysReturn(LOW);
		When(Method(ArduinoFake(), analogRead)).AlwaysReturn(123);

		When(Method(ArduinoFake(), cli)).AlwaysReturn();
		When(Method(ArduinoFake(), sei)).AlwaysReturn();

		When(Method(ArduinoFake(), micros)).AlwaysReturn(100000);
		When(Method(ArduinoFake(), millis)).AlwaysReturn(200000);


		When(Method(ArduinoFake(), randomSeed)).AlwaysDo([](unsigned long)->void{
			srand(time(NULL));
		});
		
		When(OverloadedMethod(ArduinoFake(), random, long(long))).AlwaysDo([](long upper)->long{
			return rand() % upper;
		});
		When(OverloadedMethod(ArduinoFake(), random, long(long, long))).AlwaysDo([](long lower, long upper)->long{
			return (rand() % (upper - lower)) + lower;
		});
	}

#endif
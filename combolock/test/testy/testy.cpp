#include <ArduinoFake.h>
#include <unity.h>

using namespace fakeit;

void testa()
{
	Serial.println("Foo");
}

void setUp(void)
{
    ArduinoFakeReset();
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(testa);

    UNITY_END();

    return 0;
}
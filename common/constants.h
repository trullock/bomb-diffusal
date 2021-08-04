#ifndef constants_h
#define constants_h

// pseudo state to represent I2C error
#define STATE_NONE 255

#define STATE_STRIKING 254
#define STATE_EXPLODING 253
#define STATE_DEACTIVATED 252
#define STATE_DISARMED 251

#define COMMAND_ARM 1
#define COMMAND_DIFFICULTY 2
#define COMMAND_EXPLODE 3
#define COMMAND_STRIKE 4
#define COMMAND_TIME 5

#define DIFFICULTY_LOW 1
#define DIFFICULTY_MED 2
#define DIFFICULTY_HIGH 3

#define NOTIFICATION_DISARMED 1

#define STRIKE_DURATION_MS 3000

#endif
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

#define STRIKE_DURATION_MS 3000

#define MODULE_MAX_SFX_QUEUE_LENGTH 8

#endif
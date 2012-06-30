// general.h - general setup information (global vars, version, etc)

#ifndef _GENERAL_H_
#define _GENERAL_H_

#define VERSION                 1.00
#define	PROTOCOL_VERSION       	1.00

#define PITCH                   0               // up / down
#define YAW                     1               // left / right
#define ROLL                    2               // fall over

#define ON_EPSILON              0.1             // point on plane side epsilon

#define SOUND_CHANNELS          16              // number of channels

// PHYSICS
float   gravity;

float   incspeed;
float   maxspeed;
float   decspeed;

float   friction;
float   waterfriction;

#endif

#ifndef _3D_H_
#define _3D_H_

//DEFINES////////////////////////////////////////////////////////////

// #define NOCLIP

//TYPEDEFS///////////////////////////////////////////////////////////

typedef struct
{
  float x, y, z;
} VECTOR;

typedef struct
{
  int x, y, z;
} ANG_VECT;

typedef struct
{
  VECTOR v;
  float d;
} PLANE;

typedef struct
{
  VECTOR pos;
  ANG_VECT dir;
  float incspeed, decspeed, maxspeed;
  int fov;
  VECTOR currentspeed;
  int jumping;
  int floorz;
  MATRIX_f matrix;
  PLANE frustrum[4];
} CAMERA;


//EXTERNAL GLOBAL VARIABLES//////////////////////////////////////////

//EXTERNAL FUNCTIONS/////////////////////////////////////////////////

extern void init_camera(CAMERA *);
extern void update_camera(CAMERA *);
extern float dist2_to_cam(CAMERA *, VECTOR *);

#endif
// INCLUDES
#include <math.h>
#include <allegro.h>

#include "3d.h"
#include "bsp.h"
#include "engine.h"

// DEFINES
#define B2R 0.02454369261
#define B2D 1.40625

void init_camera(CAMERA *cam) {                // init the camera
  cam->incspeed = (float) (SCREEN_W) / 91.5;
  cam->maxspeed = (float) (SCREEN_W) / 22.5;
  cam->decspeed = cam->incspeed / 1.5;
  cam->fov = 64;

  cam->currentspeed.x = 0;
  cam->currentspeed.y = 0;
  cam->currentspeed.z = 0;

  set_projection_viewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

// UPDATE CAMERA ANGLE
inline void update_camera_ang(CAMERA *cam) {
  cam->dir.x &= 255;
  cam->dir.y &= 255;
  cam->dir.z &= 255;
}

// UPDATE CAMERA SPEEDS
inline void update_camera_speeds(CAMERA *cam) {
  if (cam->currentspeed.x <= -cam->maxspeed)
    cam->currentspeed.x = -cam->maxspeed;

  if (cam->currentspeed.x >= cam->maxspeed)
    cam->currentspeed.x = cam->maxspeed;

  if (cam->currentspeed.y <= -cam->maxspeed)
    cam->currentspeed.y = -cam->maxspeed;
  if (cam->currentspeed.y >= cam->maxspeed)
    cam->currentspeed.y = cam->maxspeed;

  if (cam->currentspeed.x > cam->decspeed)
    cam->currentspeed.x -= cam->decspeed;
  else {
    if (cam->currentspeed.x < -cam->decspeed)
      cam->currentspeed.x += cam->decspeed;
    else
	  cam->currentspeed.x = 0.0;
  }

  if (cam->currentspeed.y > cam->decspeed)
    cam->currentspeed.y -= cam->decspeed;
  else {
    if (cam->currentspeed.y < -cam->decspeed)
      cam->currentspeed.y += cam->decspeed;
    else
      cam->currentspeed.y = 0.0;
  }
}

// UPDATE CAMERA POSITION
inline void update_camera_pos(CAMERA *cam) {
  // INTERNAL VARIABLES
  float cz, sz;

  cz = cos(cam->dir.z * B2R); sz = sin(cam->dir.z * B2R);

  cam->pos.x += sz * cam->currentspeed.y;

  if (!is_in_level(cam))
    cam->pos.x -= sz * cam->currentspeed.y;

  cam->pos.y += cz * cam->currentspeed.y;

  if (!is_in_level(cam))
    cam->pos.y -= cz * cam->currentspeed.y;

  cam->pos.x += cz * cam->currentspeed.x;

  if (!is_in_level(cam))
    cam->pos.x -= cz * cam->currentspeed.x;

  cam->pos.y -= sz * cam->currentspeed.x;

  if (!is_in_level(cam))
    cam->pos.y += sz * cam->currentspeed.x;

  cam->pos.z += cam->currentspeed.z;

//  if (!is_in_level(cam))
//    cam->pos.z -= cam->currentspeed.z;
}

// UPDATE CAMERA MATERIX
inline void update_camera_matrix(CAMERA *cam) {
  // INTERNAL VARIABLES
  static float cx, sx;
  static float cz, sz;
  static MATRIX_f temp;
  static VECTOR front, up;

  cx = cos(cam->dir.x * B2R); sx = sin(cam->dir.x * B2R);
  cz = cos(cam->dir.z * B2R); sz = sin(cam->dir.z * B2R);

  front.x = sz * cx;
  front.y = cz * cx;
  front.z = -sx;

  get_vector_rotation_matrix_f(&temp, front.x, front.y, front.z, cam->dir.y);
  apply_matrix_f(&temp, 0, 0, 1, &up.x, &up.y, &up.z);
  get_camera_matrix_f(&cam->matrix, 0, 0, 0, front.x, front.y, front.z, up.x, up.y, up.z, cam->fov, 1);
}

// UPDATE CAMERA FRUSTRUM
inline void update_camera_frustrum(CAMERA *cam) {
  // INTERNAL VARIABLES
  static float frust[3][3];
  static float cx, sx, cy, sy, cz, sz;
  static float cf, sf;
  static float angle;
 
  cx = cos(cam->dir.x * B2R); sx = sin(cam->dir.x * B2R);
  cy = cos(cam->dir.y * B2R); sy = sin(cam->dir.y * B2R);
  cz = cos(cam->dir.z * B2R); sz = sin(cam->dir.z * B2R);

  frust[0][0] = cz * cy + sy * sx * sz;
  frust[0][1] = -sz * cy + sy * sx * cz;
  frust[0][2] = sy * cx;

  frust[1][0] = sz * cx;
  frust[1][1] = cx * cz;
  frust[1][2] = -sx;

  frust[2][0] = sz * sx * cy - sy * cz;
  frust[2][1] = cy * cz * sx + sy * sz;
  frust[2][2] = cx * cy;

  angle = cam->fov * 0.5 * B2R;
  cf = cos(angle);
  sf = sin(angle);

  cam->frustrum[0].v.x = sf * frust[1][0] - cf * frust[0][0];
  cam->frustrum[0].v.y = sf * frust[1][1] - cf * frust[0][1];
  cam->frustrum[0].v.z = sf * frust[1][2] - cf * frust[0][2];
  cam->frustrum[0].d = cam->frustrum[0].v.x * cam->pos.x + cam->frustrum[0].v.y * cam->pos.y + cam->frustrum[0].v.z * cam->pos.z;

  cam->frustrum[1].v.x = sf * frust[1][0] + cf * frust[0][0];
  cam->frustrum[1].v.y = sf * frust[1][1] + cf * frust[0][1];
  cam->frustrum[1].v.z = sf * frust[1][2] + cf * frust[0][2];
  cam->frustrum[1].d = cam->frustrum[1].v.x * cam->pos.x + cam->frustrum[1].v.y * cam->pos.y + cam->frustrum[1].v.z * cam->pos.z;

  angle = atan((sf / cf) * 0.75);
  cf = cos(angle);
  sf = sin(angle);

  cam->frustrum[2].v.x = sf * frust[1][0] + cf * frust[2][0];
  cam->frustrum[2].v.y = sf * frust[1][1] + cf * frust[2][1];
  cam->frustrum[2].v.z = sf * frust[1][2] + cf * frust[2][2];
  cam->frustrum[2].d = cam->frustrum[2].v.x * cam->pos.x + cam->frustrum[2].v.y * cam->pos.y + cam->frustrum[2].v.z * cam->pos.z;

  cam->frustrum[3].v.x = sf * frust[1][0] - cf * frust[2][0];
  cam->frustrum[3].v.y = sf * frust[1][1] - cf * frust[2][1];
  cam->frustrum[3].v.z = sf * frust[1][2] - cf * frust[2][2];
  cam->frustrum[3].d = cam->frustrum[3].v.x * cam->pos.x + cam->frustrum[3].v.y * cam->pos.y + cam->frustrum[3].v.z * cam->pos.z;
}

// UPDATE CAMERA (ANGLE, SPEEDS, POSITION, MATRIX, FRUSTRUM)
inline void update_camera(CAMERA *cam) {
  update_camera_ang(cam);
  update_camera_speeds(cam);
  update_camera_pos(cam);
  update_camera_matrix(cam);
  update_camera_frustrum(cam);
}

// DISTANCE TO THE CAMERA
inline float dist2_to_cam(CAMERA *cam, VECTOR *in) {
  VECTOR temp;
  temp.x = in->x - cam->pos.x;
  temp.y = in->y - cam->pos.y;
  temp.z = in->z - cam->pos.z;

  return ((temp.x * temp.x) + (temp.y * temp.y) + (temp.z * temp.z));
}
// END OF FILE
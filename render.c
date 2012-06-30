// INCLUDES
#include <math.h>
#include <float.h>
#include <allegro.h>
#include "3d.h"
#include "bsp.h"
#include "cache.h"
#include "engine.h"
#include "loadbsp.h"
#include "polygon.h"
#include "render.h"

// GLOBAL VARIABLES
static V3D_f _pts1[32], _pts2[32], _ptstemp[32];
static V3D_f *vlist1[32], *vlist2[32], *vtemp[32];
static int out[32];
static BMP tex;

// INIT THE POINT LIST
static void setup_default_point_list(void) {
  // INTERNAL VARIABLES
  int i;

  // SETUP
  for (i = 0; i < 32; i++) {
    vlist1[i] = &_pts1[i];
    vlist2[i] = &_pts2[i];
    vtemp[i] = &_ptstemp[i];
  }
}

// SET CLIPPING CODE
static void set_clip_code(V3D_f *v) {
  // DO IT
  v->c = 0;
  if (v->x < -v->z)
    v->c |= 1;
  else if (v->x > v->z)
    v->c |= 2;
  if (v->y < -v->z)
    v->c |= 4;
  else if (v->y > v->z)
    v->c |= 8;
  if (v->z < 0.1)
    v->c |= 16;
}

// COMPUTE MIP-MAP LEVEL
static int compute_mip_level(int n) {
  // INTERNAL VARIABLES
  int i;
  float tex_area = 0.0, screen_area = 0.0;
  float m;

  // MIP LEVEL IS COMPUTED BASED ON THE RATIO OF AREA OCCUPIED IN TEXTURE
  // SPACE TO AREA OCCUPIED IN SCREEN SPACE. THE LARGER THE RATIO, THE
  // SMALLER THE MIP MAP MUST BE.
  for (i = 0; i < n - 1; i++) {
    tex_area    += (vlist2[i]->u * vlist2[i + 1]->v) - (vlist2[i + 1]->u * vlist2[i]->v);
    screen_area += (vlist2[i]->x * vlist2[i + 1]->y) - (vlist2[i + 1]->x * vlist2[i]->y);
  }
  tex_area    += (vlist2[n - 1]->u * vlist2[0]->v) - (vlist2[0]->u * vlist2[n - 1]->v);
  screen_area += (vlist2[n - 1]->x * vlist2[0]->y) - (vlist2[0]->x * vlist2[n - 1]->y);
  screen_area /= (float) ((SCREEN_WIDTH * SCREEN_HEIGHT) / (SCREEN_W * SCREEN_H));
  m = fabs(tex_area / screen_area);
  if (m <= 8.0)
    return 0;
  if (m <= 16.0)
    return 1;
  if (m <= 32.0)
    return 2;
  return 3;
}

// RENDER A POLYGONAL FACE
void draw_face(CAMERA *cam, int face) {
  // INTERNAL VARIABLES
  int n = dfaces[face].numedges;
  int se = dfaces[face].firstedge;
  int codes_or = 0, codes_and = 0xff;
  int i, t, edge;
  int mip, type;
  VECTOR *vertex;
  CACHED_FACE *info = &bsp_face_cache[face];
  int old87;

  // SETUP POINTERS
  setup_default_point_list();

  for (i = 0; i < n; i++) {
    // GET WORLDSPACE COORDS
    edge = dsurfedges[se + i];
    if (edge < 0)
      vertex = (VECTOR *) VERTEX(dedges[-edge].v[1]);
    else
      vertex = (VECTOR *) VERTEX(dedges[ edge].v[0]);
    // TRANSFORM TO CAMERA SPACE
    apply_matrix_f(&cam->matrix, vertex->x - cam->pos.x, vertex->y - cam->pos.y, vertex->z - cam->pos.z, &vlist1[i]->x, &vlist1[i]->y, &vlist1[i]->z);

    set_clip_code(vlist1[i]);
    codes_or  |= vlist1[i]->c;
    codes_and &= vlist1[i]->c;
  }

  // TRIVIAL CLIP
  if (codes_and)
    return;

  // GET MAPPING COORDINATES
  t = dfaces[face].texinfo;
  for (i = 0; i < n; i++) {
    edge = dsurfedges[se + i];
    if (edge < 0)
      vertex = (VECTOR *) VERTEX(dedges[-edge].v[1]);
    else
      vertex = (VECTOR *) VERTEX(dedges[ edge].v[0]);
    vlist1[i]->u = vertex->x * texinfo[t].vecs[0][0] + vertex->y * texinfo[t].vecs[0][1] + vertex->z * texinfo[t].vecs[0][2] + texinfo[t].vecs[0][3];
    vlist1[i]->v = vertex->x * texinfo[t].vecs[1][0] + vertex->y * texinfo[t].vecs[1][1] + vertex->z * texinfo[t].vecs[1][2] + texinfo[t].vecs[1][3];
  }
  // PARTIALLY IN SCREEN SPACE, NEEDS CLIPPING
  if (codes_or)
    n = clip3d_f(POLYTYPE_PTEX, 0.1, 0.1, n, vlist1, vlist2, vtemp, out);
  // NO CLIPPING, COMPLETELY IN THE SCREEN
  else
    for (i = 0; i < n; i++)
      vlist2[i] = vlist1[i];

  if (n > 2) {
    // PERSPECTIVE PROJECTION
    for (i = 0; i < n; i++) {
      persp_project_f(vlist2[i]->x, vlist2[i]->y, vlist2[i]->z, &vlist2[i]->x, &vlist2[i]->y);
      vlist2[i]->x = (int) MIN(SCREEN_WIDTH  - 1.5, MAX(0.5, vlist2[i]->x + 0.5));
      vlist2[i]->y = (int) MIN(SCREEN_HEIGHT - 1.5, MAX(0.5, vlist2[i]->y + 0.5));
    }
    // GET TEXTURE VIA SURFACE CACHE
    mip = compute_mip_level(n);
    type = get_surf(face, mip, &tex.bmp, &tex.w, &tex.h);
    // ADJUST MAPPING COORDINATES TO MIP LEVEL
    for (i = 0; i < n; i++) {
      float scale = (float) 1.0f / (1 << mip);
      if (type == WALL_TYPE) {
        vlist2[i]->u = (float) (vlist2[i]->u - info->min_s) * scale;
        vlist2[i]->v = (float) (vlist2[i]->v - info->min_t) * scale;
        // TEXTURE CLAMPING
        vlist2[i]->u = (int) MIN(tex.w - 1, MAX(0, vlist2[i]->u + 0.5));
        vlist2[i]->v = (int) MIN(tex.h - 1, MAX(0, vlist2[i]->v + 0.5));
      } else {
        vlist2[i]->u = (float) vlist2[i]->u * scale;
        vlist2[i]->v = (float) vlist2[i]->v * scale;
      }
    }
    old87 = _control87(PC_53 | RC_CHOP, MCW_PC | MCW_RC);
    render_poly(vlist2, n, type, mip, &tex);
    _control87(old87, MCW_PC | MCW_RC);
  }
}
// END OF FILE
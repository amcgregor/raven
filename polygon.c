#include <math.h>
#include <allegro.h>
#include "3d.h"
#include "engine.h"
#include "loadbsp.h"
#include "polygon.h"

#define SWIM_AMPLITUDE                          4.0
#define FXPT_SHIFT                              18
#define FXPT_MULT                               (1 << FXPT_SHIFT)

typedef struct {
  int num_pix, mip_level;
  float left_invz, right_invz;
  float left_uinvz, right_uinvz;
  float left_vinvz, right_vinvz;
  byte *scr_start;
} textured_line;

static BMP *texture;
static int swim[256];

void animate_textures(void) {
  static int swim_phase = 0;
  int i;

  swim_phase += 2;
  for (i = 0; i < 256; i++)
    swim[i] = FXPT_MULT * (SWIM_AMPLITUDE * cos((i + swim_phase) * M_PI / 128.0));
}

inline void textured_line_draw(textured_line *line) {
  static float inv_16dx, invz_16dx, uinvz_16dx, vinvz_16dx;
  static float next_invz, next_uinvz, next_vinvz;
  static float z_left, u_left, v_left;
  static float z_right, u_right, v_right;
  register fixed u, v, du = 0, dv = 0;
  static fixed next_u, next_v;
  static int n_subdiv, n_oddpixels, counter, texw;
  static byte *scr, *tex;
  static int uint, vint;

  scr = line->scr_start;
  tex = texture->bmp;
  texw = texture->w;

  inv_16dx = 16.0f / line->num_pix;
  invz_16dx = (line->right_invz - line->left_invz) * inv_16dx;
  uinvz_16dx = (line->right_uinvz - line->left_uinvz) * inv_16dx;
  vinvz_16dx = (line->right_vinvz - line->left_vinvz) * inv_16dx;

  next_invz = line->left_invz + invz_16dx;
  next_uinvz = line->left_uinvz + uinvz_16dx;
  next_vinvz = line->left_vinvz + vinvz_16dx;

  z_left = 1.0 / line->left_invz;
  u_left = line->left_uinvz * z_left;
  v_left = line->left_vinvz * z_left;
  u = (fixed) (u_left * FXPT_MULT);
  v = (fixed) (v_left * FXPT_MULT);

  n_subdiv = line->num_pix >> 4;
  n_oddpixels = line->num_pix & 15;

  while (n_subdiv-- > 0) {
    z_right = 1.0 / next_invz;
    u_right = z_right * next_uinvz;
    v_right = z_right * next_vinvz;
    next_u = (fixed) (u_right * FXPT_MULT);
    next_v = (fixed) (v_right * FXPT_MULT);
    du = (next_u - u) >> 4;
    dv = (next_v - v) >> 4;
    for (counter = 16; counter; counter--) {
      uint = u >> FXPT_SHIFT;
      vint = v >> FXPT_SHIFT;

      *(scr++) = *(tex + (vint * texw) + uint);

      u += du;
      v += dv;
    }
    z_left = z_right;
    u_left = u_right;
    v_left = v_right;
    u = next_u;
    v = next_v;
    next_invz += invz_16dx;
    next_uinvz += uinvz_16dx;
    next_vinvz += vinvz_16dx;
  }
  
  if (n_oddpixels) {
    z_right = 1.0 / line->right_invz;
    u_right = line->right_uinvz * z_right;
    v_right = line->right_vinvz * z_right;

    next_u = (fixed) (u_right * FXPT_MULT);
    next_v = (fixed) (v_right * FXPT_MULT);

    if (--n_oddpixels) {
      du = (next_u - u) / n_oddpixels;
      dv = (next_v - v) / n_oddpixels;
    }
    
    for (counter = n_oddpixels + 1; counter; counter--) {
      int uint = u >> FXPT_SHIFT;
      int vint = v >> FXPT_SHIFT;

      *(scr++) = *(tex + (vint * texw) + uint);

      u += du;
      v += dv;
    }
  }
}

inline void textured_line_draw_swirl(textured_line *line) {
  static float inv_16dx, invz_16dx, uinvz_16dx, vinvz_16dx;
  static float next_invz, next_uinvz, next_vinvz;
  static float z_left, u_left, v_left;
  static float z_right, u_right, v_right;
  register fixed u, v, du = 0, dv = 0;
  static fixed next_u, next_v;
  static int n_subdiv, n_oddpixels, counter, texw, m;
  static byte *scr, *tex;
  static int uint, vint;

  scr = line->scr_start;
  tex = texture->bmp;
  texw = 0;
  while ((1 << texw) != texture->w)
    texw++;
  m = line->mip_level;

  inv_16dx = 16.0f / line->num_pix;
  invz_16dx = (line->right_invz - line->left_invz) * inv_16dx;
  uinvz_16dx = (line->right_uinvz - line->left_uinvz) * inv_16dx;
  vinvz_16dx = (line->right_vinvz - line->left_vinvz) * inv_16dx;

  next_invz = line->left_invz + invz_16dx;
  next_uinvz = line->left_uinvz + uinvz_16dx;
  next_vinvz = line->left_vinvz + vinvz_16dx;

  z_left = 1.0 / line->left_invz;
  u_left = line->left_uinvz * z_left;
  v_left = line->left_vinvz * z_left;
  u = (fixed) (u_left * FXPT_MULT);
  v = (fixed) (v_left * FXPT_MULT);
  u += swim[(v >> (FXPT_SHIFT - m - 1)) & 0xff] >> m;
  v += swim[(u >> (FXPT_SHIFT - m - 1)) & 0xff] >> m;

  n_subdiv = line->num_pix >> 4;
  n_oddpixels = line->num_pix & 15;

  while (n_subdiv-- > 0) {
    z_right = 1.0 / next_invz;
    u_right = z_right * next_uinvz;
    v_right = z_right * next_vinvz;
    next_u = (fixed) (u_right * FXPT_MULT);
    next_v = (fixed) (v_right * FXPT_MULT);
    next_u += swim[(next_v >> (FXPT_SHIFT - m - 1)) & 0xff] >> m;
    next_v += swim[(next_u >> (FXPT_SHIFT - m - 1)) & 0xff] >> m;

    du = (next_u - u) >> 4;
    dv = (next_v - v) >> 4;
    for (counter = 16; counter; counter--) {
      uint = (u >> FXPT_SHIFT) & (texture->w - 1);
      vint = (v >> FXPT_SHIFT) & (texture->h - 1);

      *(scr++) = *(tex + (vint << texw) + uint);

      u += du;
      v += dv;
    }
    z_left = z_right;
    u_left = u_right;
    v_left = v_right;
    u = next_u;
    v = next_v;
    next_invz += invz_16dx;
    next_uinvz += uinvz_16dx;
    next_vinvz += vinvz_16dx;
  }
  if (n_oddpixels) {
    z_right = 1.0 / line->right_invz;
    u_right = line->right_uinvz * z_right;
    v_right = line->right_vinvz * z_right;

    next_u = (fixed) (u_right * FXPT_MULT);
    next_v = (fixed) (v_right * FXPT_MULT);
    next_u += swim[(next_v >> (FXPT_SHIFT - m - 1)) & 0xff] >> m;
    next_v += swim[(next_u >> (FXPT_SHIFT - m - 1)) & 0xff] >> m;
    if (--n_oddpixels) {
      du = (next_u - u) / n_oddpixels;
      dv = (next_v - v) / n_oddpixels;
    }
    for (counter = n_oddpixels + 1; counter; counter--) {
      uint = (u >> FXPT_SHIFT) & (texture->w - 1);
      vint = (v >> FXPT_SHIFT) & (texture->h - 1);

      *(scr++) = *(tex + (vint << texw) + uint);

      u += du;
      v += dv;
    }
  }
}

inline void textured_line_draw_sky(textured_line *line) {
  static float inv_16dx, invz_16dx, uinvz_16dx, vinvz_16dx;
  static float next_invz, next_uinvz, next_vinvz;
  static float z_left, u_left, v_left;
  static float z_right, u_right, v_right;
  register fixed u, v, du = 0, dv = 0;
  static fixed next_u, next_v;
  static int n_subdiv, n_oddpixels, counter, texw;
  static byte *scr, *tex;
  static int uint, vint;

  scr = line->scr_start;
  tex = texture->bmp;
  texw = 0;
  while ((1 << texw) != texture->w)
    texw++;

  inv_16dx = 16.0f / line->num_pix;
  invz_16dx = (line->right_invz - line->left_invz) * inv_16dx;
  uinvz_16dx = (line->right_uinvz - line->left_uinvz) * inv_16dx;
  vinvz_16dx = (line->right_vinvz - line->left_vinvz) * inv_16dx;

  next_invz = line->left_invz + invz_16dx;
  next_uinvz = line->left_uinvz + uinvz_16dx;
  next_vinvz = line->left_vinvz + vinvz_16dx;

  z_left = 1.0 / line->left_invz;
  u_left = line->left_uinvz * z_left;
  v_left = line->left_vinvz * z_left;
  u = (fixed) (u_left * FXPT_MULT);
  v = (fixed) (v_left * FXPT_MULT);

  n_subdiv = line->num_pix >> 4;
  n_oddpixels = line->num_pix & 15;

  while (n_subdiv-- > 0) {
    z_right = 1.0 / next_invz;
    u_right = z_right * next_uinvz;
    v_right = z_right * next_vinvz;
    next_u = (fixed) (u_right * FXPT_MULT);
    next_v = (fixed) (v_right * FXPT_MULT);
    du = (next_u - u) >> 4;
    dv = (next_v - v) >> 4;
    for (counter = 16; counter; counter--) {
      uint = (u >> FXPT_SHIFT) & ((texture->w >> 1) - 1);
      vint = (v >> FXPT_SHIFT) & (texture->h - 1);

      *(scr++) = *(tex + (vint << texw) + uint);

      u += du;
      v += dv;
    }
    z_left = z_right;
    u_left = u_right;
    v_left = v_right;
    u = next_u;
    v = next_v;
    next_invz += invz_16dx;
    next_uinvz += uinvz_16dx;
    next_vinvz += vinvz_16dx;
  }
  
  if (n_oddpixels) {
    z_right = 1.0 / line->right_invz;
    u_right = line->right_uinvz * z_right;
    v_right = line->right_vinvz * z_right;

    next_u = (fixed) (u_right * FXPT_MULT);
    next_v = (fixed) (v_right * FXPT_MULT);

    if (--n_oddpixels) {
      du = (next_u - u) / n_oddpixels;
      dv = (next_v - v) / n_oddpixels;
    }
    
    for (counter = n_oddpixels + 1; counter; counter--) {
      uint = (u >> FXPT_SHIFT) & ((texture->w >> 1) - 1);
      vint = (v >> FXPT_SHIFT) & (texture->h - 1);

      *(scr++) = *(tex + (vint << texw) + uint);

      u += du;
      v += dv;
    }
  }
}

inline void render_poly(V3D_f **vtx, int num_verts, int type, int mip, BMP *tex) {
  static int lp, y, top_vert, bottom_vert;
  static int next_vert, current_vert;
  static int edge_top_y, edge_bottom_y;
  static int int_x, i_top, i_bottom;
  static float top, bottom, dx, dy, dx_dy, x_prestep, y_prestep;
  static byte *scr_ptr;
  static byte *scan_start;
  static int pix_count, scan_add;
  static float dinvz, dinvz_dy, invz, duinvz, dvinvz, duinvz_dy, dvinvz_dy, uinvz, vinvz, dinvz_dx, duinvz_dx, dvinvz_dx, invdy, invdx, x;
  static float p_dinvz_dy, p_duinvz_dy, p_dvinvz_dy;
  static textured_line t_line;

  // span list
  struct edge {
    float left_x,right_x;
    float left_invz,right_invz;
    float left_uinvz,right_uinvz;
    float left_vinvz,right_vinvz;
  } edges[768];

  // find top and bottom vertices
  texture = tex;
  top_vert = bottom_vert = 0;
  top = vtx[0]->y;
  bottom = vtx[0]->y;
  vtx[0]->z = 1.0f / vtx[0]->z;
  vtx[0]->u *= vtx[0]->z;
  vtx[0]->v *= vtx[0]->z;
  for (lp = 1; lp < num_verts; lp++) {
    // see if this is above the current top vertex
    if (vtx[lp]->y < top) {
      top = (float) vtx[lp]->y;
      top_vert = lp;
    }
    // see if this is below the current bottom vertex
    if (vtx[lp]->y > bottom) {
      bottom = (float) vtx[lp]->y;
      bottom_vert = lp;
    }
    vtx[lp]->z = 1.0f / vtx[lp]->z;
    vtx[lp]->u *= vtx[lp]->z;
    vtx[lp]->v *= vtx[lp]->z;
  }
  // get polygon gradients, which are the same for every span and edge
  invdx = 1 / ((vtx[1]->x - vtx[2]->x) * (vtx[0]->y - vtx[2]->y) - (vtx[0]->x - vtx[2]->x) * (vtx[1]->y - vtx[2]->y));
  invdy = -invdx;
  p_dinvz_dy = invdy * ((vtx[1]->z - vtx[2]->z) * (vtx[0]->x - vtx[2]->x) - (vtx[0]->z - vtx[2]->z) * (vtx[1]->x - vtx[2]->x));
  p_duinvz_dy = invdy * ((vtx[1]->u - vtx[2]->u) * (vtx[0]->x - vtx[2]->x) - (vtx[0]->u - vtx[2]->u) * (vtx[1]->x - vtx[2]->x));
  p_dvinvz_dy = invdy * ((vtx[1]->v - vtx[2]->v) * (vtx[0]->x - vtx[2]->x) - (vtx[0]->v - vtx[2]->v) * (vtx[1]->x - vtx[2]->x));

  // get these extents into integer form, rounding down
  i_top = (int) ceil(top);
  i_bottom = (int) ceil(bottom);

  // don't bother with 1 pixel high polys
  if (i_top == i_bottom)
    return;

  // process the left edges
  current_vert = top_vert;
  do {
    // get the two vertices defining this edge, wrapping around if necessary
    next_vert = current_vert - 1;
    if (next_vert < 0)
      next_vert = num_verts - 1;
    edge_top_y = (int) ceil(vtx[current_vert]->y);
    edge_bottom_y = (int) ceil(vtx[next_vert]->y);

    // make absolutely sure no divide overflow can occur
    if (edge_bottom_y - edge_top_y > 0) {
      // find the amount of y rounding
      y_prestep = (float) edge_top_y - vtx[current_vert]->y;
      // calculate the slope and initial position
      dy = vtx[next_vert]->y - vtx[current_vert]->y;
      dx = vtx[next_vert]->x - vtx[current_vert]->x;
      duinvz = vtx[next_vert]->u - vtx[current_vert]->u;
      dvinvz = vtx[next_vert]->v - vtx[current_vert]->v;
      dinvz = vtx[next_vert]->z - vtx[current_vert]->z;
      invdy = 1.0f / dy;
      invdx = 1.0f / dx;
      dx_dy = dx * invdy;
      dinvz_dy = dinvz * invdy;
      duinvz_dy = duinvz * invdy;
      dvinvz_dy = dvinvz * invdy;

      dinvz_dx = dinvz * invdx;
      duinvz_dx = duinvz * invdx;
      dvinvz_dx = dvinvz * invdx;
      x = vtx[current_vert]->x + y_prestep * dx_dy;
      x_prestep = x - vtx[current_vert]->x;
      invz = vtx[current_vert]->z + y_prestep * dinvz_dy;
      uinvz = vtx[current_vert]->u + y_prestep * duinvz_dy;
      vinvz = vtx[current_vert]->v + y_prestep * dvinvz_dy;

      // go through every scanline, except the last one
      for (lp = edge_top_y; lp < edge_bottom_y; lp++) {
        // store the current vals
        edges[lp].left_x = x;
        edges[lp].left_invz = invz;
        edges[lp].left_uinvz = uinvz;
        edges[lp].left_vinvz = vinvz;
        // move to the next x position
        x += dx_dy;
        invz += dinvz_dy;
        uinvz += duinvz_dy;
        vinvz += dvinvz_dy;
      }
    }
    // move round to the next vertex
    current_vert = next_vert;
  } while (current_vert != bottom_vert);

  // process the right edges
  current_vert = top_vert;
  do {
    // get the two vertices defining this edge, wrapping around if necessary
    next_vert = current_vert + 1;
    if (next_vert >= num_verts)
      next_vert = 0;
    edge_top_y = (int) ceil(vtx[current_vert]->y);
    edge_bottom_y = (int) ceil(vtx[next_vert]->y);

    // make absolutely sure no divide by zero can occur
    if (edge_bottom_y - edge_top_y > 0) {
      // find the amount of y rounding
      y_prestep = (float) edge_top_y - vtx[current_vert]->y;

      // calculate the slope and initial position
      dy = vtx[next_vert]->y - vtx[current_vert]->y;
      dx = vtx[next_vert]->x - vtx[current_vert]->x;
      duinvz = vtx[next_vert]->u - vtx[current_vert]->u;
      dvinvz = vtx[next_vert]->v - vtx[current_vert]->v;
      dinvz = vtx[next_vert]->z - vtx[current_vert]->z;
      invdy = 1.0f / dy;
      dx_dy = dx * invdy;
      dinvz_dy = dinvz * invdy;
      duinvz_dy = duinvz * invdy;
      dvinvz_dy = dvinvz * invdy;
      x = vtx[current_vert]->x + y_prestep * dx_dy;
      x_prestep = x - vtx[current_vert]->x;
      invz = vtx[current_vert]->z + y_prestep * dinvz_dy;
      uinvz = vtx[current_vert]->u + y_prestep * duinvz_dy;
      vinvz = vtx[current_vert]->v + y_prestep * dvinvz_dy;

      // go through every scanline, except the last one
      for (lp = edge_top_y; lp < edge_bottom_y; lp++) {
        //Store the current vals
        edges[lp].right_x = x;
        edges[lp].right_invz = invz;
        edges[lp].right_uinvz = uinvz;
        edges[lp].right_vinvz = vinvz;
        // move to the next x position
        x += dx_dy;
        invz += dinvz_dy;
        uinvz += duinvz_dy;
        vinvz += dvinvz_dy;
      }
    }
    // move round to the next vertex
    current_vert = next_vert;
  } while (current_vert != bottom_vert);

  //get ready for moving down scanlines
  scan_add = SCREEN_WIDTH;
  scan_start = (byte *) output_buffer->line[i_top];//camera->bitmap + i_top * scan_add;

  for (y = i_top; y < i_bottom; y++) {
    // get the left-hand, no. of pixels, and the screen ptr
    int_x = (int) ceil(edges[y].left_x);
    pix_count = (int) (edges[y].right_x - int_x) + 1;
    t_line.scr_start = scan_start + int_x;

    // precision problems seem to result in spans the wrong way round
    if (pix_count > 0) {
      t_line.scr_start = scr_ptr = output_buffer->line[y] + int_x;
      dx = (float) pix_count;
      invz = edges[y].left_invz;
      uinvz = edges[y].left_uinvz;
      vinvz = edges[y].left_vinvz;
      invdx = 1.0f / dx;
      dinvz_dx = (edges[y].right_invz - invz) * invdx;
      duinvz_dx = (edges[y].right_uinvz - uinvz) * invdx;
      dvinvz_dx = (edges[y].right_vinvz - vinvz) * invdx;

      // fill in the structure for this scanline
      invdx = 1.0f / dx;
      t_line.left_invz = edges[y].left_invz;
      t_line.left_uinvz = edges[y].left_uinvz;
      t_line.left_vinvz = edges[y].left_vinvz;
      t_line.right_invz = edges[y].right_invz;
      t_line.right_uinvz = edges[y].right_uinvz;
      t_line.right_vinvz = edges[y].right_vinvz;
      t_line.num_pix = pix_count;
      t_line.mip_level = mip;

      // draw it using the correct rasteriser
      switch (type) {
        case WALL_TYPE: textured_line_draw(&t_line); break;
        case SWIRL_TYPE: textured_line_draw_swirl(&t_line); break;
        case SKY_TYPE:  textured_line_draw_sky(&t_line); break;
        default: break;
      }
      
      // move to the next scanline
      scan_start += scan_add;
    }
  }
}

//INCLUDES///////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro.h>
#include "engine.h"

#ifndef MIN
#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#define MID(x,y,z)   MAX((x), MIN((y), (z)))
#endif

#ifndef ABS
#define ABS(x)       (((x) >= 0) ? (x) : (-(x)))
#endif

#ifndef SGN
#define SGN(x)       (((x) >= 0) ? 1 : -1)
#endif

//GLOBALS VARIABLES//////////////////////////////////////////////////

byte colormap[64][256];
PALETTE pal;
BITMAP *output_buffer;
int render_wireframe = 1;

//FUNCTIONS//////////////////////////////////////////////////////////

void error(char *message, char *file, int line) { 
  allegro_exit();
  printf("Error (%s line %d): %s\n", file, line, message);
  exit(1);
}

/////////////////////////////////////////////////////////////////////

void load_pal(char *pal_file) {
  int i;
  FILE *f;

  if ((f = fopen(pal_file, "rb")) == NULL)
    fatal("Unable to open palette file");
  for (i = 0; i < 256; i++)
  {
    pal[i].r = getc(f) >> 2;
    pal[i].g = getc(f) >> 2;
    pal[i].b = getc(f) >> 2;
  }
  fclose(f);
  set_palette(pal);
}

/////////////////////////////////////////////////////////////////////

void load_colormap(char *cmap_file) {
  FILE *f;

  if ((f = fopen(cmap_file, "rb")) == NULL)
    fatal("Unable to open colormap file");
  fread(colormap, 256, 64, f);
  fclose(f);
}

/////////////////////////////////////////////////////////////////////

void engine_init(char *cfg_file, int mult) {
  set_config_file(cfg_file);

  output_buffer = create_bitmap(SCREEN_W*mult, SCREEN_H*mult);
  clear(output_buffer);

  load_pal("palette.dat");
  load_colormap("colormap.dat");
}

/////////////////////////////////////////////////////////////////////

void engine_exit(void) {
//  destroy_bitmap(output_buffer);
}

/////////////////////////////////////////////////////////////////////
//END OF FILE////////////////////////////////////////////////////////

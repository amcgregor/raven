#ifndef _LOADBSP_H_
#define _LOADBSP_H_

#define WALL_TYPE                       0
#define SWIRL_TYPE                      1
#define SKY_TYPE                        2

typedef struct
{
  int min_s, min_t;
  int size_s, size_t;
  int cache_offset;
} CACHED_FACE;

typedef struct
{
  int w, h;
  unsigned char *bmp;
} BMP;

typedef struct
{
  BMP tex[4];
  int type;
} MIPMAP;

extern CACHED_FACE *bsp_face_cache;
extern MIPMAP *mipmap_table;

extern void cache_bsp(void);
extern void load_bspworld(char *);
extern void get_player_cam(CAMERA *);
extern void unload_bspworld(void);

#endif

#ifndef _CACHE_H_
#define _CACHE_H_

#define MAX_CACHE_SIZE                          4 * 1024 * 1024

typedef struct
{
  int face_idx, mip_level;
  int width, height;
} SURF_HEADER;

extern void init_cache(void);
extern void shutdown_cache(void);
extern int get_surf(int f, int mip, unsigned char **ptr, int *w, int *h);

#endif

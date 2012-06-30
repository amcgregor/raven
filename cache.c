#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include "3d.h"
#include "bsp.h"
#include "cache.h"
#include "engine.h"
#include "loadbsp.h"

static unsigned char *surf_cache;
static int surf_cache_size, surf_cache_head, surf_cache_end, surf_cache_tail;

void init_cache(void)
{
  //Make a surface cache as big as possible

  surf_cache_size = MAX_CACHE_SIZE;
  surf_cache = NULL;

  while (!surf_cache && (surf_cache_size > 1024 * 1024))
  {
    surf_cache = (unsigned char *) malloc(surf_cache_size);
    if (!surf_cache) //Too big, try 1mb less ...
      surf_cache_size -= (1024 * 1024);
  }
  if (!surf_cache)
    fatal("Unable to allocate surface cache");
  //Set up pointers to cache
  surf_cache_head = 0;
  surf_cache_end  = 0;
  surf_cache_tail = surf_cache_size;

  memset(surf_cache, 0, surf_cache_size);
}

void shutdown_cache(void)
{
  if (surf_cache)
    free(surf_cache);
}

int get_surf(int f, int mip, unsigned char **ptr, int *w, int *h)
{
  dface_t *face;
  texinfo_t *tex;
  CACHED_FACE *info;
  SURF_HEADER *head;
  unsigned char *src_line, *src, *dest, *cache, *light, *light_line;
  int dest_x, dest_y, src_y, src_x, count_x, light_x;
  int lmap_width, lmap_height, s,ds,weight,u0,v0;
  int width, height, src_width, src_height, size;
  int light_step, light_step2, amb_light;

  face = &dfaces[f];
  info = &bsp_face_cache[f];
  tex = &texinfo[face->texinfo];

  //Special surface, no lightmaps, get directly for texture database.
  if (mipmap_table[tex->miptex].type != WALL_TYPE)
  {
    *w = mipmap_table[tex->miptex].tex[mip].w;
    *h = mipmap_table[tex->miptex].tex[mip].h;
    *ptr = mipmap_table[tex->miptex].tex[mip].bmp;
    return mipmap_table[tex->miptex].type;
  }

  //Try to find if the surf is already in cache
  head = (SURF_HEADER *) (surf_cache + info->cache_offset);
  if (head->face_idx == f && head->mip_level == mip)
  {
    *w = head->width;
    *h = head->height;
    *ptr = surf_cache + info->cache_offset + sizeof(SURF_HEADER);
    return WALL_TYPE;
  }
  //If texture is too large
  if (info->size_s > 256 || info->size_t > 256)
    return -1;

  width = info->size_s >> mip;
  height = info->size_t >> mip;
  *w = width;
  *h = height;

  // Find place for the surface in the cache
  size = width * height + sizeof(SURF_HEADER);
  // Do we need to remove some surfs from cache?
  if (surf_cache_tail + size > surf_cache_head)
  {
    // If cache wrap, start from beginning
    if (surf_cache_tail + size > surf_cache_size)
    {
      surf_cache_end = surf_cache_tail;
      surf_cache_head = surf_cache_tail = 0;
    }
    // Start removing surfaces, until enough space is free
    while(surf_cache_tail + size > surf_cache_head)
    {
      if (surf_cache_head >= surf_cache_end)
        surf_cache_head = surf_cache_size;
      else
      {
        head = (SURF_HEADER *) (surf_cache + surf_cache_head);
        surf_cache_head += head->width * head->height + sizeof(SURF_HEADER);
      }
    }
  }
  // The cache entry starts at SurfTail
  cache = surf_cache + surf_cache_tail;
  info->cache_offset = surf_cache_tail;
  surf_cache_tail += size;
  if (surf_cache_tail > surf_cache_end)
    surf_cache_end = surf_cache_tail;

// Init pointers
  head = (SURF_HEADER *) cache;
  cache += sizeof(SURF_HEADER);

// Fill out the cell header
  head->face_idx = f;
  head->mip_level = mip;
  head->width = width;
  head->height = height;

// Start offset of the texture
  u0 = info->min_s;
  v0 = info->min_t;

  u0 %= mipmap_table[tex->miptex].tex[0].w;
  v0 %= mipmap_table[tex->miptex].tex[0].h;;
  //signed mod
  if (u0 < 0)
    u0 += mipmap_table[tex->miptex].tex[0].w;
  if (v0 < 0)
    v0 += mipmap_table[tex->miptex].tex[0].h;;

  u0 >>= mip;
  v0 >>= mip;


  src_width = mipmap_table[tex->miptex].tex[mip].w;;
  src_height = mipmap_table[tex->miptex].tex[mip].h;
  src_line = mipmap_table[tex->miptex].tex[mip].bmp;

  light_step = 16 >> mip;
  light_step2 = 4 - mip;
  lmap_width = (info->size_s >> 4) + 1;
  lmap_height = (info->size_t >> 4) + 1;

  amb_light = 255; //face->styles[1];

  dest = cache;

  if (face->lightofs >= 0)
    light_line = dlightdata + face->lightofs;
  else
    light_line = NULL;

  if (light_line == NULL) //No lightmap: apply constant shading
  {
    // Tile the texture into the surface cache
    dest_y = height;
    src_y = src_height-v0;
    src_line += v0 * src_width;

    while (dest_y > 0)
    {
      // Tile horizontal line
      dest_x = width;
      src_x  = src_width - u0;
      src = src_line + u0;
      while (dest_x > 0)
      {
        if (src_x <= 0)
        {
          src = src_line;
          src_x = src_width;
        }

	count_x = MIN(src_x, dest_x);

	dest_x -= count_x;
	src_x  -= count_x;

        while (count_x > 0)
        {
          *(dest++) = *(src++);
	  count_x--;
        }
      }
      // Advance ptr to the src_line
      src_y--; src_line += src_width;
      // src_line wrap
      if (src_y <= 0)
      {
        src_line -= src_width * src_height;
        src_y = src_height;
      }
      dest_y--;
    }
  }
  else //We have a lightmap, shade the texture accordingly
  {
    //Tile the texture into the surface cache
    dest_y = height;
    src_y = src_height - v0;
    src_line += v0 * src_width;
    weight = light_step;
    while (dest_y > 0)
    {
      //Tile horizontal line
      dest_x = width;
      light = light_line;
      ds = (light[0] * weight + light[lmap_width] * (light_step - weight)) >> light_step2;
      ds = (amb_light - ds) << 14;
      light_x = 0;
      s = ds;
      src_x = src_width - u0;
      src = src_line + u0;
      while (dest_x > 0)
      {
        if (light_x <= 0)
        {
          light++;
          light_x = light_step;
          ds = (light[0] * weight + light[lmap_width] * (light_step - weight)) >> light_step2;
          ds = (amb_light - ds) << 14;
          ds = (ds - s) >> light_step2;
        }
	if (src_x <= 0)
	{
	  src = src_line;
	  src_x = src_width;
        }
        count_x = MIN(src_x, dest_x);
        if (count_x > light_x)
          count_x = light_x;
	dest_x -= count_x;
        src_x-=count_x;
	light_x-=count_x;
        while (count_x > 0)
        {
	  *dest = colormap[s >> 16][*src];
          src++;
          dest++;
	  s += ds;
          count_x--;
        }
      }
      //Advance ptr to the source line
      src_y--; src_line+=src_width;
      // src_line wrap
      if (src_y <= 0)
      {
        src_line -= src_width * src_height;
        src_y=src_height;
      }
      dest_y--;
      weight--;
      if (weight == 0)
      {
        weight = light_step;
        light_line += lmap_width;
      }
    }
  }
  *ptr = cache;

  return WALL_TYPE;
}


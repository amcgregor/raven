// INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <allegro.h>
#include "3d.h"
#include "bsp.h"
#include "cache.h"
#include "engine.h"
#include "loadbsp.h"

// GLOBAL VARIABLES
CACHED_FACE *bsp_face_cache;
MIPMAP *mipmap_table;
static dheader_t *header;

// LOAD BSP MAP FILE
static int load_file(char *filename, void **bufferptr) {
  // INTERNAL VARIABLES
  FILE *f;
  int length, pos;
  void *buffer;

  // OPEN THE FILE
  f = fopen(filename, "rb");
  if (!f) fatal("Error opening file");

  // SEEK THROUGH THE FILE
  pos = ftell(f);
  fseek(f, 0, SEEK_END);
  length = ftell(f);
  fseek(f, pos, SEEK_SET);

  // ALLOCATE A LARGE ENOUGH BUFFER TO HANDLE THE FILE
  buffer = malloc(length + 1);
  ((char *) buffer)[length] = 0;
  if (fread(buffer, 1, length, f) != (size_t) length)
    fatal("File read failure");
  fclose (f);

  // CREATE A POINTER TO THE FILE BUFFER
  *bufferptr = buffer;

  // RETURN THE LENGTH OF THE BUFFER
  return length;
}

// GET THE VALUE OF A ENTITY
static int get_entity_value(char *entity, char *field_name, char *buf) {
  // INTERNAL VARIABLES
  char *start_loc;
  char *end_loc;
  char *loc;
  int i;
  char field[512];
  
  // FIND THE ENTITY AND VALUE
  sprintf(field, "\"%s\"", field_name);
  loc = strstr(entity, field);
  if (!loc)
    return 0;
  loc++;
  
  for(i = 0; i < 2; i++) {
    loc = strchr(loc, '\"');
    if (!loc)
      fatal("Error: invalid entity found\n");
    loc++;
  }

  while (*loc == ' ' || *loc == '  ')
    loc++;
  start_loc = loc;
  while (*loc != '\"' && *loc != '\0')
    loc++;
  if (*loc == '\0')
    fatal("Error: invalid entity found\n");
  loc--;
  while (*loc == ' ' || *loc == '  ')
    loc--;
  end_loc = loc;

  for(loc = start_loc; loc <= end_loc;)
    *buf++ = *loc++;
  *buf = '\0';

  // RETURN THE VALUE
  return (end_loc - start_loc + 1);
}

// GET A STRING FROM AN ENTITY
static int get_entity_string(char *data, char *buf, char **end) {
  // INTERNAL VARIABLES
  char *start_buf;
  char c;
  int num_starts;
  char *loc;
  int num;

  // SEARCH FOR ENTITY
  while (*data != '{' && *data != '}' && *data != '\0')
    data++;
  if (*data == '}')
    fatal("Error: invalid entity found\n");
  else
    if (*data == '\0')
      return 0;

  // GET STRING
  start_buf = buf;
  num_starts = 0;
  while (1) {
    c = *data++;
    if (c == '\0')
      fatal("Error: invalid entity found\n");
    if (c == '{') {
      if (num_starts)
        fatal("Error: invalid entity found\n");
      else
        num_starts++;
    }
   *buf++ = c;
   if (c == '}')
     break;
  }
  *buf = '\0';
  num = 0;
  loc = start_buf;
  while (*loc != '\0')
    if (*loc++ == '\"')
      num++;

  if (num % 4)
    fatal("Error: invalid entity found\n");

  // RETURN THE VALUE
  *end = data;
  return (buf - start_buf);
}

// GET THE PLAYER'S START LOCATION
static void get_player_start(char *ent_data, CAMERA *cam) {
  // INTERNAL VARIABLES
  char ent_str[1024];
  char type[512];
  char val[512];
  char arg[512];
  float angle;

  // SEARCH FOR THE START LOCATION IN THE MAP, SET ALL RELAVANT INFORMATION
  while (get_entity_string(ent_data, ent_str, &ent_data)) {
    if (!get_entity_value(ent_str, "classname", type))
      continue;
    if (!strcmp(type, "info_player_start")) {
      if (get_entity_value(ent_str, "origin", val))
        sscanf(val, "%f %f %f", &cam->pos.x, &cam->pos.y, &cam->pos.z);
    }
    if (get_entity_value(ent_str, "angle", arg)) {
      if (sscanf(arg, "%f", &angle))
      {
        angle -= 90.0;
        cam->dir.z = (int) ((angle * 256.0) / 360.0);
        cam->dir.z &= 255;
      }
    }
  }
  cam->dir.x = 0;
  cam->pos.z += 20;
  cam->dir.y = 0;
}

// COPY ONE LUMP TO ANOTHER
static int copy_lump(int lump, void **dest, int size) {
  // INTERNAL VARIABLES
  int length, ofs;
  void *temp;

  // GET THE LENGTH OF THE FILE LUMPS
  length = header->lumps[lump].filelen;
  ofs = header->lumps[lump].fileofs;
   
  // CHECK TO SEE IF THE SIZE IS ODD, IF SO, FATAL
  if (length % size)
    fatal("LoadBSPFile: odd lump size");

  // ALLOCATE ENOUGH MEMORY
  temp = malloc(length);
  if (!temp)
    fatal("LoadBSPFile: not enough memory");
  memcpy (temp, (unsigned char *) header + ofs, length);

  // RETURN LENGTH OF LUMP
  *dest = temp;
  return length / size;
}

// CACHE THE BSP FILE
void cache_bsp(void) {
  // INTERNAL VARIABLES
  int i, j;
  VECTOR *vertex;
  dface_t *face;
  CACHED_FACE *cface;
  int se, edge, tex;
  float s, t;
  float min_s, min_t;
  float max_s, max_t;
  int temp1, temp2;
  dmiptexlump_t *header;
  miptex_t *mip;
  byte *bitmapdata;
  int w, h;

  // ALLOCATE MEMORY
  bsp_face_cache = (CACHED_FACE *) malloc(numfaces * sizeof(CACHED_FACE));

  // CHECK TO SEE IF THERE WAS ENOUGH
  if (bsp_face_cache == NULL)
    fatal("Not enough memory to allocate cache");

  // LOAD THE FILE
  for (i = 0; i < numfaces; i++) {
    face = &dfaces[i];
    cface = &bsp_face_cache[i];
    se = face->firstedge;
    tex = face->texinfo;

    min_s =  1e7; min_t =  1e7;
    max_s = -1e7; max_t = -1e7;
    for (j = 0; j < face->numedges; j++) {
      edge = dsurfedges[se + j];
      if (edge < 0)
        vertex = (VECTOR *) &dvertexes[dedges[-edge].v[1]];
      else
        vertex = (VECTOR *) &dvertexes[dedges[edge].v[0]];
      s = vertex->x * texinfo[tex].vecs[0][0] + vertex->y * texinfo[tex].vecs[0][1] + vertex->z * texinfo[tex].vecs[0][2] + texinfo[tex].vecs[0][3];
      t = vertex->x * texinfo[tex].vecs[1][0] + vertex->y * texinfo[tex].vecs[1][1] + vertex->z * texinfo[tex].vecs[1][2] + texinfo[tex].vecs[1][3];
      min_s = MIN(s, min_s); min_t = MIN(t, min_t);
      max_s = MAX(s, max_s); max_t = MAX(t, max_t);
    }

    temp1 = (int) floor(min_s / 16.0f);
    temp2 = (int) ceil(max_s / 16.0f);

    cface->min_s = temp1 << 4;
    cface->size_s = (temp2 - temp1) << 4;

    temp1 = (int) floor(min_t / 16.0f);
    temp2 = (int) ceil(max_t / 16.0f);

    cface->min_t = temp1 << 4;
    cface->size_t = (temp2 - temp1) << 4;

    cface->cache_offset = 0;
  }

  header = (dmiptexlump_t *) dtexdata;
  mipmap_table = (MIPMAP *) malloc(header->nummiptex * sizeof(MIPMAP));
  if (!mipmap_table)
    fatal("Not enough memory for mipmap table");
  for (i = 0; i < header->nummiptex; i++) {
    mip = (miptex_t *) (dtexdata + header->dataofs[i]);
    if (mip->width > 512 || mip->height > 512)
      continue;
    for (j = 0; j < 4; j++)
    {
      w = mip->width >> j;
      h = mip->height >> j;
      mipmap_table[i].tex[j].w = w;
      mipmap_table[i].tex[j].h = h;
      bitmapdata = (byte *) mip + mip->offsets[j];
      mipmap_table[i].tex[j].bmp = bitmapdata;
    }
    mipmap_table[i].type = WALL_TYPE;
    if (mip->name[0] == '*')
      mipmap_table[i].type = SWIRL_TYPE;
    if (mip->name[0] == 's' && mip->name[1] == 'k' && mip->name[2] == 'y')
      mipmap_table[i].type = SKY_TYPE;
  }
}

// MAP LOADER
void load_bspworld(char *filename) {
  // LOAD THE FILE
  fprintf(stdout, "  load_file(filename, (void **) &header);");
  load_file(filename, (void **) &header);

  // CHECK TO SEE IF CORRECT BSP VERSION -- NEED TO CHANGE VERSION TO RAVEN'S
//  if (header->version != BSPVERSION)
//    fatal("Incorect bspversion");

  // GRAB RELAVANT INFORMATION FROM MAP
  nummodels = copy_lump(LUMP_MODELS, (void **) &dmodels, sizeof(dmodel_t));
  fprintf(stdout, "  nummodels = %d\n", nummodels);
  numvertexes = copy_lump(LUMP_VERTEXES, (void **) &dvertexes, sizeof(dvertex_t));
  fprintf(stdout, "  numvertexes = %d\n", numvertexes);
  numplanes = copy_lump(LUMP_PLANES, (void **) &dplanes, sizeof(dplane_t));
  fprintf(stdout, "  numplanes = %d\n", numplanes);
  numleafs = copy_lump(LUMP_LEAFS, (void **) &dleafs, sizeof(dleaf_t));
  fprintf(stdout, "  numleafs = %d\n", numleafs);
  vis_leaf = (char *) malloc((numleafs / 8) + 1);
  numnodes = copy_lump(LUMP_NODES, (void **) &dnodes, sizeof(dnode_t));
  fprintf(stdout, "  numnodes = %d\n", numnodes);
  vis_node = (char *) malloc(numnodes);
  numtexinfo = copy_lump(LUMP_TEXINFO, (void **) &texinfo, sizeof(texinfo_t));
  fprintf(stdout, "  numtexinfo = %d\n", numtexinfo);
  numclipnodes = copy_lump(LUMP_CLIPNODES, (void **) &dclipnodes, sizeof(dclipnode_t));
  fprintf(stdout, "  numclipnodes = %d\n", numclipnodes);
  numfaces = copy_lump(LUMP_FACES, (void **) &dfaces, sizeof(dface_t));
  fprintf(stdout, "  numfaces = %d\n", numfaces);
  vis_face = (char *) malloc((numfaces / 8) + 1);
  nummarksurfaces = copy_lump(LUMP_MARKSURFACES, (void **) &dmarksurfaces, sizeof(dmarksurfaces[0]));

  numsurfedges = copy_lump(LUMP_SURFEDGES, (void **) &dsurfedges, sizeof(dsurfedges[0]));

  numedges = copy_lump(LUMP_EDGES, (void **) &dedges, sizeof(dedge_t));
  fprintf(stdout, "  numedges = %d\n", numedges);
  texdatasize = copy_lump(LUMP_TEXTURES, (void **) &dtexdata, 1);
  fprintf(stdout, "  texdatasize = %d\n", texdatasize);
  visdatasize = copy_lump(LUMP_VISIBILITY, (void **) &dvisdata, 1);
  fprintf(stdout, "  visdatasize = %d\n", visdatasize);
  lightdatasize = copy_lump(LUMP_LIGHTING, (void **) &dlightdata, 1);
  fprintf(stdout, "  lightdatasize = %d\n", lightdatasize);
  entdatasize = copy_lump(LUMP_ENTITIES, (void **) &dentdata, 1);
  fprintf(stdout, "  entdatasize = %d\n", entdatasize);
  free (header);

  // TEXTURE CACHE
  fprintf(stdout, "  init_cache();\n");
  init_cache();

  // BSP FACE CACHE
  fprintf(stdout, "  cache_bsp();\n");
  cache_bsp();
}

// GET PLAYER CAMERA (INIT CAMERA, GET START LOCATION)
void get_player_cam(CAMERA *cam) {
  // INIT CAMERA OBJECT
  fprintf(stdout, "  init_camera(cam);\n");
  init_camera(cam);

  // GET START LOCATION
  fprintf(stdout, "  get_player_start(dentdata, cam);\n");
  get_player_start(dentdata, cam);
}

// UNLOAD THE WORLD FROM MEMORY
void unload_bspworld(void) {
  // FREE MEMORY
  free(dmodels);
  free(dvertexes);
  free(dplanes);
  free(dleafs);
  free(dnodes);
  free(texinfo);
  free(dclipnodes);
  free(dfaces);
  free(dmarksurfaces);
  free(dsurfedges);
  free(dedges);
  free(dtexdata);
  free(dvisdata);
  free(dlightdata);
  free(dentdata);
  free(vis_face);
  free(vis_leaf);
  free(vis_node);
  free(bsp_face_cache);
  free(mipmap_table);

  // FREE THE TEXTURE CACHE
  shutdown_cache();
}
// END OF FILE
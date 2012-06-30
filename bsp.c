//INCLUDES///////////////////////////////////////////////////////////

#include <string.h>
#include <allegro.h>
#include "engine.h"
#include "3d.h"
#include "bsp.h"
#include "loadbsp.h"
#include "polygon.h"
#include "render.h"

//DEFINES////////////////////////////////////////////////////////////

#define is_marked(x)     (vis_face[(x) >> 3] &   (1 << ((x) & 7)))
#define mark_face(x)     (vis_face[(x) >> 3] |=  (1 << ((x) & 7)))
#define unmark_face(x)   (vis_face[(x) >> 3] &= ~(1 << ((x) & 7)))
#define FLOAT_POSITIVE(x)   (* (int *) (x) >= 0)

//GLOBAL VARIABLES///////////////////////////////////////////////////

int nummodels;
dmodel_t *dmodels;

int visdatasize;
byte *dvisdata;

int lightdatasize;
byte *dlightdata;

int texdatasize;
byte *dtexdata;

int entdatasize;
char *dentdata;

int numleafs;
dleaf_t *dleafs;

int numplanes;
dplane_t *dplanes;

int numvertexes;
dvertex_t *dvertexes;

int numnodes;
dnode_t *dnodes;

int numtexinfo;
texinfo_t *texinfo;

int numfaces;
dface_t *dfaces;

int numclipnodes;
dclipnode_t *dclipnodes;

int numedges;
dedge_t *dedges;

int nummarksurfaces;
unsigned short  *dmarksurfaces;

int numsurfedges;
int *dsurfedges;

char *vis_face;
char *vis_leaf;
char *vis_node;

//FUNCIONS///////////////////////////////////////////////////////////

static int point_plane_test(dplane_t *plane, CAMERA *cam)
{
  return ((plane->normal[0] * cam->pos.x + plane->normal[1] * cam->pos.y + plane->normal[2] * cam->pos.z) < plane->dist + 0.01);
}

/////////////////////////////////////////////////////////////////////

int is_in_level(CAMERA *cam)
{
  int n = dmodels[0].headnode[0];
  while (n >= 0)
  {
    dnode_t *node = &dnodes[n];
    n = node->children[point_plane_test(&dplanes[node->planenum], cam)];
  }
  n = ~n;
  if (dleafs[n].visofs < 0)
    return 0;
  return n;
}

/////////////////////////////////////////////////////////////////////

static int find_leaf(CAMERA *cam)
{
  int n = dmodels[0].headnode[0];
  while (n >= 0)
  {
    dnode_t *node = &dnodes[n];
    n = node->children[point_plane_test(&dplanes[node->planenum], cam)];
  }
  return ~n;
}

/////////////////////////////////////////////////////////////////////

static int visit_visible_leaves(CAMERA *cam)
{
  int n, v, i, j;
  static int old_leaf = 0;

  n = find_leaf(cam);
  if (n == 0 || dleafs[n].visofs < 0)
  {
    old_leaf = 0;
    return 0;
  }
  if (n == old_leaf)
    return 1;
  old_leaf = n;

  memset(vis_leaf, 0, (numleafs / 8) + 1);
  v = dleafs[n].visofs;
  for (i = 1; i < numleafs;)
  {
    if (dvisdata[v] == 0)
    {
      i += 8 * dvisdata[v + 1];
      v += 2;
    }
    else
    {
      for (j = 0; j < 8; j++)
      {
        if (dvisdata[v] & (1 << j))
          vis_leaf[i >> 3] |= (1 << (i & 7));
        i++;
      }
      v++;
    }
  }
  return 1;
}

/////////////////////////////////////////////////////////////////////

static int bsp_find_visible_nodes(CAMERA *cam, int node)
{
  if (node >= 0)
  {
    vis_node[node] = !!(bsp_find_visible_nodes(cam, dnodes[node].children[0]) | bsp_find_visible_nodes(cam, dnodes[node].children[1]));
    return vis_node[node];
  }
  else
  {
    node = ~node;
    return (vis_leaf[node >> 3] & (1 << (node & 7)));
  }
}

/////////////////////////////////////////////////////////////////////

static int bbox_inside_plane(short *mins, short *maxs, PLANE *pl)
{
  short pt[3];

  if (FLOAT_POSITIVE(&pl->v.x))
    pt[0] = maxs[0];
  else
    pt[0] = mins[0];
  if (FLOAT_POSITIVE(&pl->v.y))
    pt[1] = maxs[1];
  else
    pt[1] = mins[1];
  if (FLOAT_POSITIVE(&pl->v.z))
    pt[2] = maxs[2];
  else
    pt[2] = mins[2];

  return ((pl->v.x * pt[0] + pl->v.y * pt[1] + pl->v.z * pt[2]) >= pl->d);
}

/////////////////////////////////////////////////////////////////////

static int node_in_frustrum(dnode_t *node, CAMERA *cam)
{
  if (!bbox_inside_plane(node->mins, node->maxs, &cam->frustrum[0]))
    return 0;
  if (!bbox_inside_plane(node->mins, node->maxs, &cam->frustrum[1]))
    return 0;
  if (!bbox_inside_plane(node->mins, node->maxs, &cam->frustrum[2]))
    return 0;
  if (!bbox_inside_plane(node->mins, node->maxs, &cam->frustrum[3]))
    return 0;
  return 1;
}

/////////////////////////////////////////////////////////////////////

static int leaf_in_frustrum(dleaf_t *leaf, CAMERA *cam)
{
  if (!bbox_inside_plane(leaf->mins, leaf->maxs, &cam->frustrum[0]))
    return 0;
  if (!bbox_inside_plane(leaf->mins, leaf->maxs, &cam->frustrum[1]))
    return 0;
  if (!bbox_inside_plane(leaf->mins, leaf->maxs, &cam->frustrum[2]))
    return 0;
  if (!bbox_inside_plane(leaf->mins, leaf->maxs, &cam->frustrum[3]))
    return 0;
  return 1;
}

/////////////////////////////////////////////////////////////////////

static void mark_leaf_faces(int leaf)
{
  int n = dleafs[leaf].nummarksurfaces;
  int ms = dleafs[leaf].firstmarksurface;
  int i;

  for (i = 0; i < n; i++)
  {
    int s = dmarksurfaces[ms + i];
    mark_face(s);
  }
}

/////////////////////////////////////////////////////////////////////

static void bsp_explore_node(CAMERA *cam, int node)
{
  if (node < 0)
  {
    node = ~node;
    if (vis_leaf[node >> 3] & (1 << (node & 7)))
      if (leaf_in_frustrum(&dleafs[node], cam))
        mark_leaf_faces(node);
    return;
  }
  if (vis_node[node])
  {
    if (!node_in_frustrum(&dnodes[node], cam))
      vis_node[node] = 0;
    else
    {
      bsp_explore_node(cam, dnodes[node].children[0]);
      bsp_explore_node(cam, dnodes[node].children[1]);
    }
  }
}

/////////////////////////////////////////////////////////////////////

static void render_node_faces(CAMERA *cam, int node, int side)
{
  int i, n, f;

  n = dnodes[node].numfaces;
  f = dnodes[node].firstface;
  for (i = 0; i < n; i++)
  {
    if (is_marked(f))
    {
      if (dfaces[f].side == side)
        draw_face(cam, f);
      unmark_face(f);
    }
    f++;
  }
}

/////////////////////////////////////////////////////////////////////

static int camera_in_front_of_node(CAMERA *cam, dplane_t *p)
{
  dvertex_t *c = (dvertex_t *) &cam->pos;
  switch (p->type)
  {
    case PLANE_X:
    case PLANE_Y:
    case PLANE_Z: return (c->point[p->type] < p->dist + 0.01);
    case PLANE_ANYX:
    case PLANE_ANYY:
    case PLANE_ANYZ:
    default: return ((p->normal[0] * c->point[0] + p->normal[1] * c->point[1] + p->normal[2] * c->point[2]) < p->dist + 0.01);
  }
}

/////////////////////////////////////////////////////////////////////

static void bsp_render_node(CAMERA *cam, int node)
{
  if (node >= 0 && vis_node[node])
  {
    if (camera_in_front_of_node(cam, &dplanes[dnodes[node].planenum]))
    {
      bsp_render_node(cam, dnodes[node].children[0]);
      render_node_faces(cam, node, 1);
      bsp_render_node(cam, dnodes[node].children[1]);
    }
    else
    {
      bsp_render_node(cam, dnodes[node].children[1]);
      render_node_faces(cam, node, 0);
      bsp_render_node(cam, dnodes[node].children[0]);
    }
  }
}

/////////////////////////////////////////////////////////////////////

inline void render_world(CAMERA *cam)
{
  animate_textures();
  if (!visit_visible_leaves(cam)) {
    if (!is_in_level(cam)) clear(output_buffer);
    memset(vis_leaf, 255, (numleafs / 8) + 1);
  }
  bsp_find_visible_nodes(cam, dmodels[0].headnode[0]);
  bsp_explore_node(cam, dmodels[0].headnode[0]);
  bsp_render_node(cam, dmodels[0].headnode[0]);
}

/////////////////////////////////////////////////////////////////////
//END OF FILE////////////////////////////////////////////////////////

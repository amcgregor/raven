#ifndef _BSP_H_
#define _BSP_H_

//DEFINES////////////////////////////////////////////////////////////

#define VERTEX(x)           &dvertexes[x]
#define	MAX_MAP_HULLS		4

#define	MAX_MAP_MODELS		256
#define	MAX_MAP_BRUSHES		4096
#define	MAX_MAP_ENTITIES	1024
#define	MAX_MAP_ENTSTRING	65536

#define	MAX_MAP_PLANES		8192
#define	MAX_MAP_NODES		32767		// because negative shorts are contents
#define	MAX_MAP_CLIPNODES	32767		//
#define	MAX_MAP_LEAFS		32767		// 
#define	MAX_MAP_VERTS		65535
#define	MAX_MAP_FACES		65535
#define	MAX_MAP_MARKSURFACES 65535
#define	MAX_MAP_TEXINFO		4096
#define	MAX_MAP_EDGES		256000
#define	MAX_MAP_SURFEDGES	512000
#define	MAX_MAP_MIPTEX		0x200000
#define	MAX_MAP_LIGHTING	0x100000
#define	MAX_MAP_VISIBILITY	0x100000
#define MAX_KEY      		32
#define MAX_VALUE   		1024
#define BSPVERSION   		29
#define LUMP_ENTITIES   	0
#define LUMP_PLANES      	1
#define LUMP_TEXTURES   	2
#define LUMP_VERTEXES   	3
#define LUMP_VISIBILITY   	4
#define LUMP_NODES      	5
#define LUMP_TEXINFO   		6
#define LUMP_FACES      	7
#define LUMP_LIGHTING   	8
#define LUMP_CLIPNODES   	9
#define LUMP_LEAFS      	10
#define LUMP_MARKSURFACES 	11
#define LUMP_EDGES      	12
#define LUMP_SURFEDGES   	13
#define LUMP_MODELS      	14
#define HEADER_LUMPS   		15
#define MAXLIGHTMAPS   		4
#define NUM_AMBIENTS        4
#define MIPLEVELS   		4

//TYPEDEFS///////////////////////////////////////////////////////////

typedef struct
{
  int fileofs, filelen;
} lump_t;

typedef struct
{
  float mins[3], maxs[3];
  float origin[3];
  int headnode[MAX_MAP_HULLS];
  int visleafs;                 // not including the solid leaf 0
  int firstface, numfaces;
} dmodel_t;

typedef struct
{
  int version;
  lump_t lumps[HEADER_LUMPS];
} dheader_t;

typedef struct
{
  int nummiptex;
  int dataofs[4];               // [nummiptex]
} dmiptexlump_t;

typedef struct miptex_s
{
  char name[16];
  unsigned width, height;
  unsigned offsets[MIPLEVELS];  // four mip maps stored
} miptex_t;

typedef struct
{
  float point[3];
} dvertex_t;

// 0-2 are axial planes
#define   PLANE_X         0
#define   PLANE_Y         1
#define   PLANE_Z         2
// 3-5 are non-axial planes snapped to the nearest
#define   PLANE_ANYX      3
#define   PLANE_ANYY      4
#define   PLANE_ANYZ      5

typedef struct
{
  float normal[3];
  float dist;
  int type;
} dplane_t;

typedef struct
{
  int planenum;
  short children[2];
  short mins[3];
  short maxs[3];
  unsigned short firstface;
  unsigned short numfaces;
} dnode_t;

typedef struct
{
  int planenum;
  short children[2];
} dclipnode_t;

typedef struct texinfo_s
{
  float vecs[2][4];
  int miptex;
  int flags;
} texinfo_t;

#define TEX_SPECIAL             1

typedef struct
{
  unsigned short v[2];
} dedge_t;

typedef struct
{
  short planenum;
  short side;
  int firstedge;
  short numedges;
  short texinfo;
  unsigned char styles[MAXLIGHTMAPS];
  int lightofs;
} dface_t;

typedef struct
{
  int contents;
  int visofs;
  short mins[3];
  short maxs[3];
  unsigned short firstmarksurface;
  unsigned short nummarksurfaces;
  unsigned char ambient_level[NUM_AMBIENTS];
} dleaf_t;

//EXTERNAL GLOBAL VARIABLES//////////////////////////////////////////

extern int nummodels;
extern dmodel_t *dmodels;

extern int visdatasize;
extern unsigned char *dvisdata;

extern int lightdatasize;
extern unsigned char *dlightdata;

extern int texdatasize;
extern unsigned char *dtexdata;

extern int entdatasize;
extern char *dentdata;

extern int numleafs;
extern dleaf_t *dleafs;

extern int numplanes;
extern dplane_t *dplanes;

extern int numvertexes;
extern dvertex_t *dvertexes;

extern int numnodes;
extern dnode_t *dnodes;

extern int numtexinfo;
extern texinfo_t *texinfo;

extern int numfaces;
extern dface_t *dfaces;

extern int numclipnodes;
extern dclipnode_t *dclipnodes;

extern int numedges;
extern dedge_t *dedges;

extern int nummarksurfaces;
extern unsigned short  *dmarksurfaces;

extern int numsurfedges;
extern int *dsurfedges;

extern char *vis_face;//[MAX_MAP_FACES / 8 + 1];
extern char *vis_leaf;//[MAX_MAP_LEAFS / 8 + 1];
extern char *vis_node;//[MAX_MAP_NODES];


//EXTERNAL FUNCTIONS/////////////////////////////////////////////////

extern int is_in_level(CAMERA *);
extern void render_world(CAMERA *cam);

#endif

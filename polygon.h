// POLYGON.H - HEADER FOR POLYGON RENDERING AND TEXTURE ANIMATION

// MAKE SHURE WE WERN'T ALLREADY INCLUDED
#ifndef _POLYGON_H_
#define _POLYGON_H_

// ANIMATE THE TEXTURES (WATER, LAVA, OOZE, PORTAL, !SKY!)
extern void animate_textures(void);

// RENDER POLYGON
extern void render_poly(V3D_f **, int, int, int, BMP *);

#endif


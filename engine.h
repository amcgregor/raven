#ifndef _ENGINE_H_
#define _ENGINE_H_

//DEFINES////////////////////////////////////////////////////////////

#define SCREEN_WIDTH                    (output_buffer->w)
#define SCREEN_HEIGHT                   (output_buffer->h)
#define fatal(s)                error(s, __FILE__, __LINE__)

//TYPEDEFS///////////////////////////////////////////////////////////

typedef unsigned char byte;

//EXTERNAL GLOBAL VARIABLES//////////////////////////////////////////

extern byte colormap[64][256];
extern PALETTE pal;
extern BITMAP *output_buffer;
extern int render_wireframe;

//EXTERNAL FUNCTIONS/////////////////////////////////////////////////

extern void error(char *, char *, int);
extern void load_pal(char *);
extern void load_colormap(char *);
extern void engine_init(char *, int);
extern void engine_exit(void);

#endif

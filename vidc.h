// VidC Video Storage Method Copyright (c) 2001 Alice Bevan-McGregor,
// All Rights Reserved.  Use of this code is restricted for
// any and all uses.  FNORD!  This is early alpha code and 
// subject to massive change at any time.  Be warned.  DANCE!

/*
#include <std_disclaimer.h>

   "I do not accept responsibility for any effects, adverse or otherwise, 
    that this code may have on you, your computer, your sanity, your dog, 
    and anything else that you can think of. Use it at your own risk."
*/

#ifdef __VIDC_H__
#define __VIDC_H__

#define fatal(s)        error(s, __FILE__, __LINE__)

#define VIDC_MARKER                  0x40 // generic marker
#define VIDC_DATA_MARK               0x40 // 64 or the @ symbol
#define VIDC_FRAME_MARK              0x40
#define VIDC_FRAME_HEADER_END_MARK   0x40
#define VIDC_FRAME_END_MARK          0x40

typedef struct tagVIDC_HEADER {
  char         header[3];                               // file type
  unsigned int version : 8;                             // file version
  unsigned int w : 16;                                  // movie width
  unsigned int h : 16;                                  // movie height
  unsigned int bpp : 8;                                 // movie color depth
  unsigned int fps : 8;                                 // movie frame rate
  unsigned int num_frames : 32;                         // max frames
  unsigned int key_frames : 8;                          // movie depth, yknow
  unsigned int start_marker : 8;                        // start movie marker
} VIDC_HEADER;

typedef struct tagVIDC_FRAME_HEADER {  
  unsigned int header : 8;                              // frame header ident
  unsigned int number : 32;	                            // 2147483647 max
  unsigned int flags : 8;                               // frame flags
  unsigned int x : 16;
  unsigned int y : 16;
  unsigned int w : 16;
  unsigned int h : 16;                                  // frame position
  unsigned int end_marker : 8;                          // end of the header
} VIDC_FRAME_HEADER;

typedef struct tagVIDC_FRAME_DATA {
  unsigned char data[];                                 // the actual data
  unsigned int frame_end_marker : 8;                    // end frame marker
} FRAME_DATA;

// BITMAP routines
BITMAP *xor_bitmap(BITMAP *dest, BITMAP *prev, BITMAP *curr);
BITMAP *subtract_bitmap(BITMAP *dest, BITMAP *prev, BITMAP *curr, int delta);
BITMAP *deframe_image(VIDC_HEADER *vheader, VIDC_FRAME_HEADER *header, int file);

// VIDC_HEADER routines
VIDC_HEADER *init_vidc_header(int w, int h, int bpp, int fps, int nf, int kf );
void destroy_vidc_header( VIDC_HEADER *header );

// VIDC_FRAME_HEADER routines
VIDC_FRAME_HEADER *init_vidc_frame_header(int fn, int f int x, int y, int w, int h);
void destroy_vidc_frame_header( VIDC_FRAME_HEADER *header );



#endif __VIDC_H__
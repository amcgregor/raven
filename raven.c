// INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <allegro.h>

#include "3d.h"
#include "bsp.h"
#include "engine.h"
#include "loadbsp.h"
//#include "console.h"
//#include "cmds.h"

// DEBUG INFORMATION, ANYONE?
//#define debug

#define MAX_SPIES 4

// global datafile variables
//DATAFILE *datData;          // these are currently not used...
//DATAFILE *datFonts;
//DATAFILE *datGraphics;
//DATAFILE *datMusic;
//DATAFILE *datSound;
//DATAFILE *datLevels;
//DATAFILE *datStrings;       // but they will be.

// GLOBAL VARIABLES GOOD!
int ScreenWidth = 320, ScreenHeight = 200;       // screen resolution
int ColorDepth = 8;                             // you should know...
#ifdef debug
int VirtualWidth = 320*2, VirtualHeight = 200*2;     // virtual screen size
char *mapname = "";                              // the map filename
int VirtualMult = 1;                             // virtual screen multiplier
#endif
volatile int frame_count = 0, fps = 0;           // frame counter vars

BITMAP *bmpBuffer;                               // the bitmap buffer
CAMERA cam;                                      // the player's camera

//#ifdef debug
void fps_proc ( ) {                              // frame counter proc
   fps = frame_count;
   frame_count = 0;
} END_OF_FUNCTION(fps_proc);
//#endif

void set_spy (CAMERA *spycam, CAMERA *playercam) {
  memcpy(spycam, playercam, sizeof(CAMERA));
}

void application_main ( void ) {                 // the main application loop
  int current_spy = 0;                           // the current spy camera
  CAMERA spy[MAX_SPIES];                         // the spy camera array
  int mouses_x = 0, mouses_y = 0;                // the mouse mickeys
  int HealthLevel = 128, ManaLevel = 128;

  // GET THE START LOCATION FOR PLAYER
  fprintf(stdout, "get_player_cam(&cam);\n");
  get_player_cam(&cam);                          // set up player camera
  for (current_spy = 0; current_spy < MAX_SPIES; current_spy++) {
    fprintf(stdout, "get_player_cam(&spy[%d]);\n", current_spy);
    get_player_cam(&spy[current_spy]);           // set up spy camera
  }
  current_spy = 0;

  // MAIN LOOP - QUIT IF THE USER HITS ESCAPE
  while (!key[KEY_ESC]) {
    // this handles finding the floor.  don't achieve escape velocity,
    // you won't like the result.  you'll fly, and keep flying.
    while (is_in_level(&cam)) cam.pos.z--;
    cam.floorz = cam.pos.z + 1;
    cam.pos.z += 45;

    // get mouse movement, the distance the mouse has moved since it was last
    // polled (or mickeys for short)
    get_mouse_mickeys(&mouses_y, &mouses_x);

    // look down, way down -- betcha can't touch yer toes =)
    if (key[KEY_PGDN]) {
      cam.dir.x += 4;
      if (cam.dir.x >= 256)
        cam.dir.x -= 256;
      if (cam.dir.x > 55 && cam.dir.x < 128)
        cam.dir.x = 55;
    }

    // look up, way up -- that's gotta hurt your neck
    if (key[KEY_PGUP]) {
      cam.dir.x -= 4;
      if (cam.dir.x < 0)
        cam.dir.x += 256;
      if (cam.dir.x < 200 && cam.dir.x > 128)
        cam.dir.x = 200;
    }

    // look straight -- that's better
    if (key[KEY_END])
      cam.dir.x = 0;

    // handle mouse movement -- change the directions on a mickey
    cam.dir.x += mouses_x >> 1;
      if (cam.dir.x >= 256)
        cam.dir.x -= 256;
      if (cam.dir.x > 55 && cam.dir.x < 128)
        cam.dir.x = 55;
      if (cam.dir.x < 0)
        cam.dir.x += 256;
      if (cam.dir.x < 200 && cam.dir.x > 128)
        cam.dir.x = 200;
    cam.dir.z += mouses_y >> 1;

    // move foreward -- hold shift to go faster
    if (key[KEY_UP] || (mouse_b & 2)) {
      if (key[KEY_LSHIFT])
        cam.currentspeed.y += cam.incspeed * 2;
      if (!key[KEY_LSHIFT])
        cam.currentspeed.y += cam.incspeed;
    }

    // move backward -- hold shift to go faster
    if (key[KEY_DOWN]) {
      if (key[KEY_LSHIFT])
        cam.currentspeed.y -= cam.incspeed * 2;
      if (!key[KEY_LSHIFT])
        cam.currentspeed.y -= cam.incspeed;
    }

    // turn / strafe right
    if (key[KEY_RIGHT]) {
      if (key[KEY_LSHIFT])                       // faster
        cam.dir.z += 8;
      if (!key[KEY_ALT] && !key[KEY_LSHIFT])     // normal
        cam.dir.z += 2;
      if (key[KEY_ALT])                          // strafe
        cam.currentspeed.x += cam.incspeed;
    }

    // turn / strafe right
    if (key[KEY_LEFT]) {
      if (key[KEY_LSHIFT])                       // faster
        cam.dir.z -= 8;
      if (!key[KEY_LSHIFT] && !key[KEY_ALT])     // normal
        cam.dir.z -= 2;
      if (key[KEY_ALT])                          // strafe
        cam.currentspeed.x -= cam.incspeed;
    }

    // float up - since there is no propper physics, this needs a lot of work
    if (key[KEY_D])
      cam.pos.z += 40;

    // float down - same here as above
    if (key[KEY_C])
      cam.pos.z -= 35;

    if (key[KEY_TILDE]) {
      current_spy = 0;
    } else if (key[KEY_1]) {
      if (key[KEY_LSHIFT]) set_spy(&spy[0], &cam);
      else current_spy = 1;
    } else if (key[KEY_2]) {
      if (key[KEY_LSHIFT]) set_spy(&spy[1], &cam);
      else current_spy = 2;
    } else if (key[KEY_3]) {
      if (key[KEY_LSHIFT]) set_spy(&spy[2], &cam);
      else current_spy = 3;
    } else if (key[KEY_4]) {
      if (key[KEY_LSHIFT]) set_spy(&spy[3], &cam);
      else current_spy = 4;
    }
    
    if (key[KEY_F2] && key[KEY_RSHIFT]) {        // yoink a screen shot
      PALETTE pal;
      get_palette(pal);
      save_bitmap("dump.bmp", screen, pal);      // needs to be fixed. try it
    }

    if (key[KEY_UP] || key[KEY_DOWN] || key[KEY_LEFT] || key[KEY_RIGHT])
      current_spy = 0;

    // field of view handlers -- zoom in and out
    // there should be limits, but this is an alpha, right?
    if (key[KEY_INSERT]) cam.fov++;
    else if (key[KEY_DEL]) cam.fov--;
    else if (key[KEY_HOME]) cam.fov = 64;

    // render the main window, and the spy-cams
    if (current_spy == 0) {                      // no spy cam selected
      update_camera(&cam);                       // update the positions
      render_world(&cam);                        // render the scene
    } else {                                     // one of them is selected
//      update_camera(&spy[current_spy-1]);        // update the cam position
      render_world(&spy[current_spy-1]);         // render the scene
    } blit(output_buffer, bmpBuffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    hline(bmpBuffer,(SCREEN_W/2)-1,(SCREEN_H/2),(SCREEN_W/2)+1,
          makecol(255,255,255));
    vline(bmpBuffer,(SCREEN_W/2),(SCREEN_H/2)-1,(SCREEN_H/2)+1,
          makecol(255,255,255));

    // MANA BAR -- TEMPORARY CHEAPNESS?
    vline(bmpBuffer, SCREEN_W-15+1, 2, 2+100, makecol(192,192,216));
    vline(bmpBuffer, SCREEN_W-15+2, 2, 2+100, makecol(192,192,216));
    vline(bmpBuffer, SCREEN_W-15+3, 2, 2+100, makecol(192,192,216));
    vline(bmpBuffer, SCREEN_W-15+4, 2, 2+100, makecol(192,192,216));
    vline(bmpBuffer, SCREEN_W-15+5, 2, 2+100, makecol(192,192,216));
    vline(bmpBuffer, SCREEN_W-15+6, 2, 2+100, makecol(192,192,216));
    vline(bmpBuffer, SCREEN_W-15+7, 2, 2+100, makecol(192,192,216));
    vline(bmpBuffer, SCREEN_W-15+8, 2, 2+100, makecol(192,192,216));
    vline(bmpBuffer, SCREEN_W-15+9, 2, 2+100, makecol(192,192,216));
    hline(bmpBuffer, SCREEN_W-15, 2, SCREEN_W-5, makecol(216,216,255));
    hline(bmpBuffer, SCREEN_W-15, 2+100, SCREEN_W-5, makecol(128, 128, 192));
    vline(bmpBuffer, SCREEN_W-15, 2, 2+100, makecol(216,216,255));
    vline(bmpBuffer, SCREEN_W-5, 2, 2+100, makecol(128, 128, 192));
    putpixel(bmpBuffer, SCREEN_W-15, 2, makecol(255,255,255));
    putpixel(bmpBuffer, SCREEN_W-15, 2+100, makecol(192,192,216));
    putpixel(bmpBuffer, SCREEN_W-5, 2, makecol(192,192,216));
    putpixel(bmpBuffer, SCREEN_W-5, 2+100, makecol(96,96,128));

    // HEALTH BAR -- TEMPORARY CHEAPNESS?
    // draw_health(x, y, w, h, val)
    vline(bmpBuffer, SCREEN_W-30+1, 2, 2+50, makecol(216,192,192));
    vline(bmpBuffer, SCREEN_W-30+2, 2, 2+50, makecol(216,192,192));
    vline(bmpBuffer, SCREEN_W-30+3, 2, 2+50, makecol(216,192,192));
    vline(bmpBuffer, SCREEN_W-30+4, 2, 2+50, makecol(216,192,192));
    vline(bmpBuffer, SCREEN_W-30+5, 2, 2+50, makecol(216,192,192));
    vline(bmpBuffer, SCREEN_W-30+6, 2, 2+50, makecol(216,192,192));
    vline(bmpBuffer, SCREEN_W-30+7, 2, 2+50, makecol(216,192,192));
    vline(bmpBuffer, SCREEN_W-30+8, 2, 2+50, makecol(216,192,192));
    vline(bmpBuffer, SCREEN_W-30+9, 2, 2+50, makecol(216,192,192));
    hline(bmpBuffer, SCREEN_W-30, 2, SCREEN_W-20, makecol(255,216,216));
    hline(bmpBuffer, SCREEN_W-30, 2+50, SCREEN_W-20, makecol(192, 128, 128));
    vline(bmpBuffer, SCREEN_W-30, 2, 2+50, makecol(255,216,216));
    vline(bmpBuffer, SCREEN_W-20, 2, 2+50, makecol(192, 128, 128));
    putpixel(bmpBuffer, SCREEN_W-30, 2, makecol(255,255,255));
    putpixel(bmpBuffer, SCREEN_W-30, 2+50, makecol(216,192,192));
    putpixel(bmpBuffer, SCREEN_W-20, 2, makecol(216,192,192));
    putpixel(bmpBuffer, SCREEN_W-20, 2+50, makecol(128,96,96));



#ifdef debug
    textprintf(bmpBuffer, font, 1, 1+text_height(font)*0, makecol(255,255,255),
               " FPS: %ld  SPY: %d", (long)fps, current_spy);

    textprintf(bmpBuffer, font, 1, 1+text_height(font)*1, makecol(255,255,255),
               "   X: %4ld  Y: %4ld  Z: %4ld  FZ: %4ld", (long)cam.pos.x,
               (long)cam.pos.y, (long)cam.pos.z, (long)cam.floorz);

    textprintf(bmpBuffer, font, 1, 1+text_height(font)*2, makecol(255,255,255),
               "DIRX: %4ld  DIRZ: %4ld", (long)cam.dir.x, (long)cam.dir.z);

    textprintf(bmpBuffer, font, 1, 1+text_height(font)*3, makecol(255,255,255),
               " MAP: %s", mapname);

    textprintf(bmpBuffer, font, 1, 1+text_height(font)*4, makecol(255,255,255),
               "XRES: %d  YRES: %d  DEPTH: %d", ScreenWidth,
               ScreenHeight, ColorDepth);

    textprintf(bmpBuffer, font, 1, 1+text_height(font)*5, makecol(255,255,255),
               "XVIR: %d  YVIR: %d", VirtualWidth, VirtualHeight);

    frame_count++;
#endif

    // copy buffer to screen -- yay, i explody
    blit(bmpBuffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
  }
}

// MAIN FUNCTION -- APPLICATION START
int main(int argc, char **argv) {
  PALETTE pal;
  clrscr();
  textbackground(1);
  textcolor(11);
  printf(" Raven3D - Core V.12-31-2000, Interface V.12-31-2000                            ");
  printf("\n");
  textbackground(0);
  textcolor(7);
  printf("Copyright (c) 2000, Alice Bevan-McGregor\n");
  printf("Compiled with DJGPP/ALLEGRO.\n");

  fprintf(stdout, "set_config_file(\"raven.cfg\");\n");
  // SET CURRENT CONFIGURATION FILE, IF NOT FOUND, USE DEFAULT SETTINGS
  set_config_file("raven.cfg");

  fprintf(stdout, "allegro_init();\n");
  // INIT ALLEGRO GAME LIBRARY
  allegro_init();

  fprintf(stdout, "install_keyboard();\n");
  // INIT KEYBOARD HANDLER
  install_keyboard();

  fprintf(stdout, "install_timer();\n");
  // INIT TIMER HANDLER
  install_timer();

  fprintf(stdout, "install_mouse();\n");
  // INIT MOUSE HANDLER -- MUST HAVE MOUSE DRIVER IN BACKGROUND!
  if (install_mouse() < 0) {
    fprintf(stderr, "\nError initializing mouse!\n");
    fprintf(stderr, "Load mouse driver before running.\n");
    return 1;
  }

//  fprintf(stdout, "  \n");

  fprintf(stdout, "check_cpu();\n");
  // GRAB CPU INFORMATION
  check_cpu();

//  set_gfx_mode(GFX_SAFE, 320, 200, 0, 0);
//  if (!gfx_mode_select(&ColorDepth, &ScreenWidth, &ScreenHeight))
//    exit(1);

  // LOAD DATAFILE INTO MEMORY -- FIND A BETTER WAY?
//  datData = load_datafile("raven.dat");
//  if (datData == NULL) {
//    printf ("Unable to load `raven.dat'!\n");
//    return 1;
//  }

  // LOAD INFORMATION FROM DATAFILE
//  datFonts = datData[FONTS].dat;
//  datGraphics = datData[GRAPHICS].dat;

  //application_splash();

  // START 3D ENGINE, CHANGE RESOLUTON, GENERAL ENGINE SETUP
  fprintf(stdout, "set_color_depth(8);\n");
  set_color_depth(8);
  fprintf(stdout, "set_gfx_mode(GFX_AUTODETECT, 320, 200, 0, 0);\n");
  set_gfx_mode(GFX_AUTODETECT, 320, 200, 0, 0);
  fprintf(stdout, "engine_init(\"raven.cfg\", 1);\n");
  engine_init("raven.cfg", 1);
  fprintf(stdout, "get_palette(pal);\n");
  get_palette(pal);
  if (argv[2]) ColorDepth = strtol(argv[2], NULL, 0);
  fprintf(stdout, "set_color_depth(%d);\n", ColorDepth);
  set_color_depth(ColorDepth);

  fprintf(stdout, "set_gfx_mode(GFX_AUTODETECT, %d, %d, 0, 0);\n", ScreenWidth, ScreenHeight);
  set_gfx_mode(GFX_AUTODETECT, ScreenWidth, ScreenHeight, 0, 0);
  set_palette(pal);

#ifdef debug
  // install frames per second timer
  fprintf(stdout, "install_int(fps_proc, 1000);\n");
  install_int(fps_proc, 1000);
#endif
  
  fprintf(stdout, "bmpBuffer = create_bitmap(%d, %d);\n", SCREEN_W, SCREEN_H);
  bmpBuffer = create_bitmap(SCREEN_W, SCREEN_H);

  // LOAD MAP
  if (argv[1]) {
    fprintf(stdout, "load_bspworld(%s);\n", argv[1]);
    load_bspworld(argv[1]);
  } else {
    fprintf(stdout, "load_bspworld(\"level.bsp\");\n");
    load_bspworld("level.bsp");
  }

#ifdef debug
  if (argv[1])
    mapname = argv[1];
  else
    mapname = "level.bsp";
#endif

  // MAKE TEXT TRANSPARENT
  fprintf(stdout, "text_mode(-1);\n");
  text_mode(-1);

  //  The Application Menu would go here... later, dude.

  // MAIN LOOP
  fprintf(stdout, "application_main();\n");
  application_main();

  // CLEAN UP BITMAPS
  destroy_bitmap(bmpBuffer);

  // EXIT ALLEGRO, UNLOAD MAP, UNLOAD ENGINE
  allegro_exit();

  clrscr();
  textbackground(1);
  textcolor(11);
  cprintf(" Raven3D - Core V.12-31-2000, Interface V.12-31-2000                            ");
  printf("\n");
  textbackground(0);
  textcolor(7);
  printf("Copyright (c) 2000, Alice Bevan-McGregor\n");
  printf("Compiled with DJGPP/ALLEGRO.\n");

  printf("unload_bspworld();\n");
  unload_bspworld();
  printf("engine_exit();\n");
  engine_exit();

  // RETURN 0 FOR NO ERROR (SUCCESS)
  return 0;
}
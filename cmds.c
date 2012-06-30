// FILE         : CMDS.C
// COMMENT      : Conains game commands (i.e. console commands and internals).
// DATE STARTED : 03 APRIL 1999.

// INCLUDES:

#include "djdoom.h"
#include "console.h"
#include "player.h"
#include "cmds.h"
#include "trig.h"
#include "gfx.h"
#include "wad_io.h"
#include "map.h"
#include "demo.h"

// FUNCTIONS:

void cmds_init(){
    // Initiates data for the console commands.

    // Command #0.
    strcpy(con_cmds[0].name,"clear");
    con_cmds[0].type='C';
    con_cmds[0].max=0;

    // Command #1.
    strcpy(con_cmds[1].name,"crosshair");
    con_cmds[1].type='I';
    con_cmds[1].mirror=player_crosshair;
    con_cmds[1].min=0;
    con_cmds[1].max=1;

    // Command #2.
    strcpy(con_cmds[2].name,"crosshair_color");
    con_cmds[2].type='I';
    con_cmds[2].mirror=player_crosshair_color;
    con_cmds[2].min=0;
    con_cmds[2].max=255;

    // Command #3.
    strcpy(con_cmds[3].name,"dir");
    con_cmds[3].type='C';
    con_cmds[3].max=2;

    // Command #4.
    strcpy(con_cmds[4].name,"disconnect");
    con_cmds[4].type='C';
    con_cmds[4].max=0;

    // Command #5.
    strcpy(con_cmds[5].name,"exec");
    con_cmds[5].type='C';
    con_cmds[5].max=1;

    // Command #6.
    strcpy(con_cmds[6].name,"floors");
    con_cmds[6].type='I';
    con_cmds[6].mirror=player_floors;
    con_cmds[6].min=0;
    con_cmds[6].max=1;

    // Command #7.
    strcpy(con_cmds[7].name,"fov");
    con_cmds[7].type='F';
    con_cmds[7].mirror=ftofix(player_fov_degrees);
    con_cmds[7].min=ftofix(10.0);
    con_cmds[7].max=ftofix(170.0);

    // Command #8.
    strcpy(con_cmds[8].name,"fps");
    con_cmds[8].type='I';
    con_cmds[8].mirror=player_fps;
    con_cmds[8].min=0;
    con_cmds[8].max=1;

    // Command #9.
    strcpy(con_cmds[9].name,"gamma");
    con_cmds[9].type='F';
    con_cmds[9].mirror=player_gamma;
    con_cmds[9].min=ftofix(0.0);
    con_cmds[9].max=ftofix(10.0);

    // Command #10.
    strcpy(con_cmds[10].name,"gfx_mode");
    con_cmds[10].type='I';
    con_cmds[10].mirror=player_gfx_mode;
    con_cmds[10].min=1;
    con_cmds[10].max=max_gfx_modes;

    // Command #11.
    strcpy(con_cmds[11].name,"gfx_modes");
    con_cmds[11].type='C';
    con_cmds[11].max=0;

    // Command #12.
    strcpy(con_cmds[12].name,"height_fix");
    con_cmds[12].type='I';
    con_cmds[12].mirror=player_heightfix;
    con_cmds[12].min=0;
    con_cmds[12].max=1;

    // Command #13.
    strcpy(con_cmds[13].name,"invert_mouse");
    con_cmds[13].type='I';
    con_cmds[13].mirror=player_invert;
    con_cmds[13].min=0;
    con_cmds[13].max=1;

    // Command #14.
    strcpy(con_cmds[14].name,"light_con");
    con_cmds[14].type='I';
    con_cmds[14].mirror=light_con;
    con_cmds[14].min=0;
    con_cmds[14].max=256;

    // Command #15.
    strcpy(con_cmds[15].name,"light_depth");
    con_cmds[15].type='F';
    con_cmds[15].mirror=ftofix(light_depth);
    con_cmds[15].min=ftofix(0.0);
    con_cmds[15].max=ftofix(100.0);

    // Command #16.
    strcpy(con_cmds[16].name,"loopdemo");
    con_cmds[16].type='C';
    con_cmds[16].max=3;

    // Command #17.
    strcpy(con_cmds[17].name,"map");
    con_cmds[17].type='C';
    con_cmds[17].max=1;

    // Command #18.
    strcpy(con_cmds[18].name,"map_info");
    con_cmds[18].type='C';
    con_cmds[18].max=0;

    // Command #19.
    strcpy(con_cmds[19].name,"map_rotate");
    con_cmds[19].type='I';
    con_cmds[19].mirror=player_map_rotate;
    con_cmds[19].min=0;
    con_cmds[19].max=1;

    // Command #20.
    strcpy(con_cmds[20].name,"map_show");
    con_cmds[20].type='I';
    con_cmds[20].mirror=player_map;
    con_cmds[20].min=0;
    con_cmds[20].max=1;

    // Command #21.
    strcpy(con_cmds[21].name,"map_size");
    con_cmds[21].type='I';
    con_cmds[21].mirror=player_map_size;
    con_cmds[21].min=5;
    con_cmds[21].max=100;

    // Command #22.
    strcpy(con_cmds[22].name,"mem");
    con_cmds[22].type='C';
    con_cmds[22].max=0;

    // Command #23.
    strcpy(con_cmds[23].name,"pic_con");
    strcpy(con_cmds[23].s_mirror,player_pic_con);
    con_cmds[23].type='S';

    // Command #24.
    strcpy(con_cmds[24].name,"playdemo");
    con_cmds[24].type='C';
    con_cmds[24].max=1;

    // Command #25.
    strcpy(con_cmds[25].name,"pwad");
    con_cmds[25].type='C';
    con_cmds[25].max=2;

    // Command #26.
    strcpy(con_cmds[26].name,"quit");
    con_cmds[26].type='C';
    con_cmds[26].max=0;

    // Command #27.
    strcpy(con_cmds[27].name,"recdemo");
    con_cmds[27].type='C';
    con_cmds[27].max=1;

    // Command #28.
    strcpy(con_cmds[28].name,"r_grad");
    con_cmds[28].type='I';
    con_cmds[28].mirror=player_r_grad;
    con_cmds[28].min=0;
    con_cmds[28].max=10000;

    // Command #29.
    strcpy(con_cmds[29].name,"r_gun");
    con_cmds[29].type='I';
    con_cmds[29].mirror=player_r_gun;
    con_cmds[29].min=0;
    con_cmds[29].max=1;

    // Command #30
    strcpy(con_cmds[30].name,"stop");
    con_cmds[30].type='C';
    con_cmds[30].max=0;

    // Command #31.
    strcpy(con_cmds[31].name,"syntax");
    con_cmds[31].type='C';
    con_cmds[31].max=1;

    // Command #32.
    strcpy(con_cmds[32].name,"time");
    con_cmds[32].type='C';
    con_cmds[32].max=0;

    // Command #33.
    strcpy(con_cmds[33].name,"timerefresh");
    con_cmds[33].type='C';
    con_cmds[33].max=0;

    // Command #34.
    strcpy(con_cmds[34].name,"trace");
    con_cmds[34].type='C';
    con_cmds[34].max=0;

    // Command #35.
    strcpy(con_cmds[35].name,"viewpic");
    con_cmds[35].type='C';
    con_cmds[35].max=1;

    // Command #36.
    strcpy(con_cmds[36].name,"viewtex");
    con_cmds[36].type='C';
    con_cmds[36].max=1;

    // Command #37.
    strcpy(con_cmds[37].name,"vsync");
    con_cmds[37].type='I';
    con_cmds[37].mirror=player_vsync;
    con_cmds[37].min=0;
    con_cmds[37].max=1;

    // Command #38.
    strcpy(con_cmds[38].name,"wads");
    con_cmds[38].type='C';
    con_cmds[38].max=0;

    // Command #39.
    strcpy(con_cmds[39].name,"walls");
    con_cmds[39].type='I';
    con_cmds[39].mirror=player_walls;
    con_cmds[39].min=0;
    con_cmds[39].max=1;

    // Command #40.
    strcpy(con_cmds[40].name,"window_size");
    con_cmds[40].type='I';
    con_cmds[40].mirror=player_windowsize;
    con_cmds[40].min=0;
    con_cmds[40].max=8;

    // The number of commands.
    max_cmds=41;
}

/* ---------------------------- */
/* INTERNAL COMMANDS START HERE */
/* ---------------------------- */

void internal_setstring(byte *q1, byte *q2){
    // Sets a string to 32 chars max.
    strncpy(q1,q2,31);
    q1[31]=0; // Just in case.
}

void internal_stopdemo(){
    // Frees the demo buffers.
    player_recording=0;
    free(bufdemo);
    con_printf("demo buffers free");
}

void internal_savedemo(){
    // Flushes the demo buf to the demo file.
    FILE *fp;
    byte fname[50];

    // Flush the demo buffer to the demo file.
    sprintf(fname,"data/demos/%s.dem",demoname);
    fp=fopen(fname,"wb");
    fwrite(&demo_header,sizeof(demo_header),1,fp);
    fwrite(bufdemo,sizeof(DEMO_CELL),demo_count,fp);
    fclose(fp);
    sprintf(saybuf,"wrote %s",fname);
    con_printf(saybuf);
}

int internal_loaddemo(){
    // Loads a demo from the demo file.
    int fs,error=1;
    FILE *fp;
    byte fname[50];

    // Read the demo buffer from the file.
    demo_count=0;
    sprintf(fname,"data/demos/%s.dem",demoname);
    fs=file_size(fname);
    if(fs==0){
        sprintf(saybuf,"couldn't load %s",fname);
        con_printf(saybuf);
    }
    else{
        // Create the demo buffer.
        demo_max=(fs-sizeof(demo_header))/sizeof(DEMO_CELL);
        bufdemo=(DEMO_CELL *)malloc(sizeof(DEMO_CELL)*demo_max);

        // Read the demo.
        fp=fopen(fname,"rb");
        fread(&demo_header,sizeof(demo_header),1,fp);
        fread(bufdemo,demo_max,sizeof(DEMO_CELL),fp);
        fclose(fp);

        // Try to load the map for the demo.
        player_connected=0;
        strcpy(mapname,demo_header.mapname);
        if(load_map(INTERNAL_WAD)==0){
            player_recording=2;
            player_connected=1;
            player_con=0;         // <-- Bring up the console.
            map_type=MAP_IWAD;
            error=0;
        }
        else{
            // Couldn't play demo, so free the memory.
            free(bufdemo);
        }
    }

    // Return the error.
    return(error);
}

void internal_loopdemo(){
    // Loads the next demo in the loop.
    loop_count++;
    if(loop_count==3){loop_count=0;}
    strcpy(demoname,loopdemo[loop_count]);
    if(internal_loaddemo()!=0){
        //con_printf("looping aborted");
        loop_count=-1;
    }
}

void internal_quitdemo(){
    // Quits the current demo.
    player_connected=0;

    // Stop play the current demo.
    internal_stopdemo();

    // If we're looping, play the next demo.
    if(loop_count!=-1){
        internal_loopdemo();
    }
}

void internal_screenshot(){
    // Dumps the screen to the files DJDOOM00.BMP, DJDOOM01.BMP, etc.
    byte fname[13];
    int i=0,got;

    // Skip through the files until we find a free one.
    do{
        sprintf(fname,"djdoom%02d.bmp",i);
        got=file_exists(fname,0,NULL);
        if(i==99){got=0;}
        if(got==0){
            save_bmp(fname,vgabuf,gamma_pal); // Take the actual screen shot.
            sprintf(saybuf,"wrote %s",fname); // Create ...
            con_printf(saybuf);               // ... and display console message.
        }
        i++;
    }while(got!=0);
}

void internal_writeconfig(){
    // Writes a config file.
    struct date d;
    FILE *fp;
    int i;

    // Open the file.
    fp=fopen("DJDOOM.CFG","wt");

    // Write the standard header.
    getdate(&d);
    fprintf(fp,"// FILE         : DJDOOM.CFG\n");
    fprintf(fp,"// COMMENT      : DJDOOM v1.0 startup config file\n");
    fprintf(fp,"// DATE CREATED : %d.%d.%d\n",d.da_day,d.da_mon,d.da_year);
    fprintf(fp,"\n");

    // Write out all the console variables.
    fprintf(fp,"// VARIABLES:\n");
    fprintf(fp,"\n");
    for(i=0;i<max_cmds;i++){
        if(con_cmds[i].type=='F'){
            // It's a FIXED variable.
            fprintf(fp,"%s %c%f%c\n",con_cmds[i].name,34,fixtof(con_cmds[i].mirror),34);
        }
        if(con_cmds[i].type=='I'){
            // It's an INT.
            fprintf(fp,"%s %c%d%c\n",con_cmds[i].name,34,con_cmds[i].mirror,34);
        }
        if(con_cmds[i].type=='S'){
            // It's a string.
            fprintf(fp,"%s %c%s%c\n",con_cmds[i].name,34,con_cmds[i].s_mirror,34);
        }
    }
    fprintf(fp,"\n");

    // Close the file.
    fclose(fp);
}

void internal_set_gfx_mode(){
    // Actually sets the graphics mode.
    int error,n=player_gfx_mode-1;

    // Try to set the graphics mode (unless we're allready in it).
    if(n!=old_gfx_mode){
        error=set_gfx_mode(GFX_AUTODETECT,gfx_mode_info[n].width,gfx_mode_info[n].height,0,0);
        if(error<0){
            // DEBUG: Will this error even appear in a bad graphics mode?
            con_printf("Couldn't initiate the graphics mode!");
        }
        else{
            // Get the new screen width/height.
            screen_width=gfx_mode_info[n].width;
            screen_height=gfx_mode_info[n].height;
            set_palette(gamma_pal);

            // DEBUG: Shouldn't we free the old VGABUF?!!
            vgabuf=create_bitmap(screen_width,screen_height);
            sprintf(saybuf,"%d x %d mode set (%dk vgabuf)",screen_width,screen_height,screen_width*screen_height/1024);
            con_printf(saybuf);

            // Reset the console height, if the console is down.
            if(player_connected==1&&player_con==1){
                con_height=screen_height/2;
            }
        }
    }

    // Save the old mode.
    old_gfx_mode=n;
}

int internal_setmapname(byte *fname){
    // Attempts to set the mapname to a given file name.
    int error=0;

    // Only accept the name if it's at most 8 chars long.
    if(strlen(fname)<=8){
        // Copy the new name, and lower it.
        strcpy(mapname,fname);
        strlwr(mapname);
    }
    else{
        error=1;
    }

    // Return 1 if error, 0 if okay.
    return(error);
}

int internal_setdemoname(byte *fname){
    // Attempts to set the mapname to a given file name.
    int error=0;

    // Only accept the name if it's at most 8 chars long.
    if(strlen(fname)<=8){
        // Copy the new name, and lower it.
        strcpy(demoname,fname);
        strlwr(demoname);
    }
    else{
        error=1;
    }

    // Return 1 if error, 0 if okay.
    return(error);
}

int internal_setwadpath(int wad_num,byte *fname){
    // Attempts to set the wadpath to a given file name.
    int error=0;

    // Add the extension ".WAD" if one hasn't allready been given.
    if(strlen(fname)<=12){
        strcpy(wad_path[wad_num],fname);
        if(strchr(wad_path[wad_num],46)==NULL&&strlen(wad_path[wad_num])<=8){
            strcat(wad_path[wad_num],".wad");
        }
        strlwr(wad_path[wad_num]);
    }
    else{
        error=1;
    }

    // Return 1 if error, 0 if okay.
    return(error);
}

void internal_error(){
    sprintf(saybuf,"Unknown command %c%s%c",34,parse_words[0],34);
    con_printf(saybuf);
}

/* --------------------------- */
/* CONSOLE COMMANDS START HERE */
/* --------------------------- */

void cmd_syntax(){
    // SYNTAX command.
    int i,got=-1;

    // Make sure we are checking the syntax of something.
    for(i=0;i<max_cmds;i++){
        if(strcmp(parse_words[1],con_cmds[i].name)==0){got=i;}
    }

    // Now display the syntax.
    if(got>=0){
        if(con_cmds[got].type=='C'){
            con_printf("  type : console command");
            sprintf(saybuf,"  args : %d",con_cmds[got].max);
            con_printf(saybuf);
        }
        if(con_cmds[got].type=='I'){
            con_printf("  type : integer variable");
            sprintf(saybuf,"  min  : %d",con_cmds[got].min);
            con_printf(saybuf);
            sprintf(saybuf,"  max  : %d",con_cmds[got].max);
            con_printf(saybuf);
        }
        if(con_cmds[got].type=='F'){
            con_printf("  type : fixed point variable");
            sprintf(saybuf,"  min  : %f",fixtof(con_cmds[got].min));
            con_printf(saybuf);
            sprintf(saybuf,"  max  : %f",fixtof(con_cmds[got].max));
            con_printf(saybuf);
        }
        if(con_cmds[got].type=='S'){
            con_printf("  type : string variable");
        }
        con_printf(" ");
    }
}

void cmd_disconnect(){
    // The command used to disconnect from a map.
    int i;

    // DEBUG: Free memory here ... e.g. map, textures.

    // Now we're really disconnected.
    player_connected=0;
    if(player_recording!=0){
        internal_stopdemo();
    }
}

void cmd_gamma(){
    // Builds GAMMA_PAL from BASE_PAL and GAMMA.
    // DEBUG: The gamma correction formula could be improved.
    int i;
    float r,g,b,delta;

    // Initiate.
    delta=128.0/(fixtof(player_gamma)+1.0)-64.0;

    // Convert each color.
    for(i=0;i<256;i++){
        // Fetch the base colors.
        r=base_pal[i].r;
        g=base_pal[i].g;
        b=base_pal[i].b;

        // Apply the gamma fix to each color-component.
        r+=delta;
        g+=delta;
        b+=delta;

        // Make sure that we're in range.
        if(r<0){r=0;}
        if(g<0){g=0;}
        if(b<0){b=0;}
        if(r>63){r=63;}
        if(g>63){g=63;}
        if(b>63){b=63;}

        // Store the new gamma colors.
        gamma_pal[i].r=r;
        gamma_pal[i].g=g;
        gamma_pal[i].b=b;
    }

    // Now set the new palette.
    set_palette(gamma_pal);
}

void cmd_gfx_mode(){
    // Sets the graphics mode.
    internal_set_gfx_mode();
}

void cmd_pic_con(){
    // Attempts to set the console picture.
    WAD_PIC *temp_pic;
    int error=1;

    // Read up the proposed picture.
    temp_pic=wad_loadpic(INTERNAL_WAD,player_pic_con);
    if(temp_pic!=NULL){
        if(temp_pic->w==320&&temp_pic->h==200){
            conpic=wad_loadpic(INTERNAL_WAD,player_pic_con);
            error=0;
        }
        wad_killpic(temp_pic);
    }
    if(error==1){
        con_printf("invalid background (titlepic reset)");
        strcpy(player_pic_con,"titlepic");
        conpic=wad_loadpic(INTERNAL_WAD,"titlepic");
        cmds_init(); // To mirror the change in pic_con.
    }
}

void cmd_viewpic(){
    // Views the contents of a WAD file picture.
    // WARNING: Needs an error check, to make sure lump is actually a picture.
    WAD_PIC *temp_pic;
    temp_pic=wad_loadpic(INTERNAL_WAD,parse_words[1]);
    if(temp_pic!=NULL){
        clear_to_color(vgabuf,0);
        draw_scaled_wad_pic(temp_pic,temp_pic->w,temp_pic->h,256,0,0,screen_height);
        vsync();
        blit(vgabuf,screen,0,0,0,0,screen_width,screen_height);
        readkey();
        wad_killpic(temp_pic);
    }
    else{
        con_printf("viewpic failed");
    }
}

void cmd_dir(){
    // Lists the WAD file directory.
    int i,n1,n2;
    n1=atoi(parse_words[1]);
    n2=atoi(parse_words[2]);
    if(n1<0){n1=0;}
    if(n2>=wad_header[INTERNAL_WAD].lumps){n2=wad_header[INTERNAL_WAD].lumps-1;}
    for(i=n1;i<=n2;i++){
        sprintf(saybuf,"    %04d  %8s  %8d bytes",i,wad_dir[INTERNAL_WAD][i].name,wad_dir[INTERNAL_WAD][i].size);
        con_printf(saybuf);
    }
}

void cmd_gfx_modes(){
    // Lists the currently supported graphics modes.
    int i;
    for(i=0;i<max_gfx_modes;i++){
        sprintf(saybuf,"  %d. %d x %d",i+1,gfx_mode_info[i].width,gfx_mode_info[i].height);
        con_printf(saybuf);
    }
}

void cmd_wads(){
    // Lists the current loaded wads.
    int i;
    for(i=0;i<hard_max_wads;i++){
        if(wad_used[i]==1){
            sprintf(newbuf,"%c%c%c%c",wad_header[i].sig[0],wad_header[i].sig[1],wad_header[i].sig[2],wad_header[i].sig[3]);
            strlwr(newbuf);
            sprintf(saybuf,"%s: %s (%d lumps)",newbuf,wad_path[i],wad_header[i].lumps);
            con_printf(saybuf);
        }
    }
}

void cmd_mem(){
    // Lists current memory usage.
    con_printf("memory statistics unavailable");
}

void cmd_quit(){
    // The command used to quit the game.
    player_game_over=1;
}

void cmd_time(){
    // Just so I can see how late it is =)
    struct date d;
    struct time t;

    // Get the date and time.
    getdate(&d);
    gettime(&t);

    // Print the date and time to the console.
    sprintf(saybuf,"%02d:%02d (%d.%d.%d)",t.ti_hour,t.ti_min,d.da_day,d.da_mon,d.da_year);
    con_printf(saybuf);
}

void cmd_clear(){
    // Clear the console buffer.
    memset(conbuf,32,sizeof(conbuf));
    memset(conget,0,sizeof(conget));
}

void cmd_map(){
    // The MAP command. Disconnects, loads a new map and connects if okay.
    cmd_disconnect();
    if(internal_setmapname(parse_words[1])==0){
        if(load_map(INTERNAL_WAD)==0){
            player_connected=1;
            player_con=0;       // <-- Bring up the console.
            map_type=MAP_IWAD;
        }
    }
    else{
        con_printf("bad map name");
    }
}

void cmd_exec(){
    // Execs a config file.
    load_exec(parse_words[1]);
}

void cmd_pwad(){
    // Disconnects, loads a patch wad and connects if okay.
    cmd_disconnect();
    if(internal_setmapname(parse_words[2])==0){
        if(internal_setwadpath(PATCH_WAD,parse_words[1])==0){
            if(wad_add(PATCH_WAD)==0){
                if(load_map(PATCH_WAD)==0){
                    player_connected=1;
                    player_con=0;         // <-- Bring up the console.
                    map_type=MAP_PWAD;
                }
            }
        }
        else{
            con_printf("bad wad name");
        }
    }
    else{
        con_printf("bad map name");
    }
}

void cmd_timerefresh(){
    // Checks how fast the engine is rendering the current scene.
    int i,timer_now,timer_count;
    float frames,secs;

    // Only do the TIMEREFRESH if we're actaully looking at a level.
    if(player_connected==1){

        // Initiate.
        timer_now=clock();
        frames=0;

        // Turn the player around full circle (128 frames in total, like Quake).
        for(i=0;i<full_circle/10;i++){
            player_angle+=10;
            if(player_angle>=full_circle){player_angle-=full_circle;}
            transform_vertices();
            window_size_init();
            clear_to_color(vgabuf,0);
            //test_render();
            render();
            if(player_vsync==1){vsync();}
            blit(vgabuf,screen,0,0,0,0,screen_width,screen_height);
            frames++;
        }

        // Work out how long it took.
        timer_count=clock();
        secs=(float)(timer_count-timer_now)/(float)CLOCKS_PER_SEC;
        sprintf(saybuf,"%f seconds (%f fps)",secs,frames/secs);
        con_printf(saybuf);
    }
    else{
        con_printf("can't timerefresh when disconnected");
    }
}

void cmd_mapinfo(){
    // Displays current map statistics.
    if(player_connected==1){
        if(map_type==MAP_IWAD){
            sprintf(saybuf,"%s from iwad %s",mapname,wad_path[INTERNAL_WAD]);
            con_printf(saybuf);
        }
        if(map_type==MAP_PWAD){
            sprintf(saybuf,"%s from pwad %s",mapname,wad_path[PATCH_WAD]);
            con_printf(saybuf);
        }
        sprintf(saybuf,"title: %s",map_title);
        con_printf(saybuf);
        con_printf("");
        sprintf(saybuf,"    %5d things"   ,max_things);   con_printf(saybuf);
        sprintf(saybuf,"    %5d linedefs" ,max_linedefs); con_printf(saybuf);
        sprintf(saybuf,"    %5d sidedefs" ,max_sidedefs); con_printf(saybuf);
        sprintf(saybuf,"    %5d vertices" ,max_vertices); con_printf(saybuf);
        sprintf(saybuf,"    %5d segs"     ,max_segs);     con_printf(saybuf);
        sprintf(saybuf,"    %5d ssectors" ,max_ssectors); con_printf(saybuf);
        sprintf(saybuf,"    %5d nodes"    ,max_nodes);    con_printf(saybuf);
        sprintf(saybuf,"    %5d sectors"  ,max_sectors);  con_printf(saybuf);
    }
    else{
        con_printf("no map, no info");
    }
}

void cmd_viewtex(){
    // Views a texture (made up of patches).
    WAD_TEX *temp_tex;

    // Try to load up and then display the texture.
    temp_tex=wad_loadpatch(parse_words[1]);
    if(temp_tex!=NULL){
        // Display texture statistics.
        sprintf(saybuf,"width %d, height %d",temp_tex->w,temp_tex->h);
        con_printf(saybuf);

        // Dump the texture to the screen.
        clear_to_color(vgabuf,0);
        draw_tex(temp_tex);
        vsync();
        blit(vgabuf,screen,0,0,0,0,screen_width,screen_height);
        readkey();
        wad_killpatch(temp_tex);
    }
    else{
        con_printf("viewtex failed");
    }
}

void cmd_loopdemo(){
    // Begins the playing of a demo.
    int error,m;

    // Check if we're allready recording/playing.
    if(player_recording!=0){
        con_printf("demo buffers are in use");
        return;
    }
    if(internal_setdemoname(parse_words[3])!=0){return;}
    strcpy(loopdemo[2],demoname);
    if(internal_setdemoname(parse_words[2])!=0){return;}
    strcpy(loopdemo[1],demoname);
    if(internal_setdemoname(parse_words[1])!=0){return;}
    strcpy(loopdemo[0],demoname);

    // Give okay message.
    //sprintf(saybuf,"looping %s %s %s",loopdemo[0],loopdemo[1],loopdemo[2]);
    //con_printf(saybuf);

    // Start off the first demo.
    loop_count=2;
    internal_loopdemo();
}

void cmd_playdemo(){
    // Begins the playing of a demo.
    int m;

    // Check if we're allready recording/playing.
    if(player_recording!=0){
        con_printf("demo buffers are in use");
        return;
    }
    if(internal_setdemoname(parse_words[1])==0){
        internal_loaddemo();
        loop_count=-1; // Just for safety.
    }
    else{
        con_printf("bad demo name");
    }
}

void cmd_recdemo(){
    // Begins recording a demo.
    int m;

    // Check if we're allready recording/playing.
    if(player_connected!=1||map_type!=MAP_IWAD){
        con_printf("not connected to an iwad map");
        return;
    }
    if(player_recording!=0){
        con_printf("demo buffers in use");
        return;
    }
    if(internal_setdemoname(parse_words[1])==0){
        // Initiate the demo.
        m=sizeof(DEMO_CELL)*10000;
        bufdemo=(DEMO_CELL *)malloc(m);
        demo_count=0;
        demo_header.ver=DJDOOM_VER;
        strcpy(demo_header.mapname,mapname);

        // Display recording statistics.
        sprintf(saybuf,"version %d demo (%s)",demo_header.ver,demo_header.mapname);
        con_printf(saybuf);
        player_recording=1;
    }
    else{
        con_printf("bad demo name");
    }
}

void cmd_stop(){
    // Stops recording a demo.
    if(player_recording!=1){
        con_printf("not recording a demo");
    }
    if(player_recording==1){
        internal_savedemo();
        internal_stopdemo();
    }
}

/* ------------------------- */
/* CONSOLE COMMANDS END HERE */
/* ------------------------- */

void view_variable(int n){
    // Displays the contents of a variable to the console.
    if(con_cmds[n].type=='F'){
        // It's a FIXED variable.
        sprintf(saybuf,"%s is %c%f%c",con_cmds[n].name,34,fixtof(con_cmds[n].mirror),34);
    }
    if(con_cmds[n].type=='I'){
        // It's an INT.
        sprintf(saybuf,"%s is %c%d%c",con_cmds[n].name,34,con_cmds[n].mirror,34);
    }
    if(con_cmds[n].type=='S'){
        // It's a string.
        sprintf(saybuf,"%s is %c%s%c",con_cmds[n].name,34,con_cmds[n].s_mirror,34);
    }
    con_printf(saybuf);
}

int exec_parsed(){
    // Execs a parsed command line.
    int error=0,i,got=-1,n;

    // Now execute the command.
    if(parse_nwords!=0){
         // Search for a matching known command.
         for(i=0;i<max_cmds;i++){
             if(strcmp(con_cmds[i].name,parse_words[0])==0){
                got=i;
             }
         }

         if(got>=0){
             // Make sure that we have the correct number of arguments for a command.
             if(con_cmds[got].type=='C'){
                 if(parse_nwords!=con_cmds[got].max+1){
                     got=-2;
                 }
             }
             // If we have a variable, deal with it here.
             else{
                 // No arguments means view the variable.
                 if(parse_nwords==1){
                     view_variable(got);
                     got=1000;
                 }

                 // One argument means set the variable.
                 if(parse_nwords==2){

                     // Find the value to set the variable to.
                     if(con_cmds[got].type=='F'){
                         // It's a FIXED variable.
                         n=ftofix(atof(parse_words[1]));
                     }
                     if(con_cmds[got].type=='I'){
                         // It's an INT.
                         n=atoi(parse_words[1]);
                     }
                     if(con_cmds[got].type=='S'){
                         // It's a string.
                         internal_setstring(con_cmds[got].s_mirror,parse_words[1]);
                     }
                     else{
                         // Put the value within limits.
                         if(n<con_cmds[got].min){n=con_cmds[got].min;}
                         if(n>con_cmds[got].max){n=con_cmds[got].max;}

                         // Set the mirror to the value.
                         con_cmds[got].mirror=n;
                     }
                 }

                 // If we don't have one or two arguments, then its an error.
                 if(parse_nwords>2){
                     got=-2;
                 }
             }
         }

         // Execute an actual command, or set a given variable.
         switch(got){
             case -1: internal_error();break;
             case  0: cmd_clear();break;
             case  1: player_crosshair=n;break;
             case  2: player_crosshair_color=n;break;
             case  3: cmd_dir();break;
             case  4: cmd_disconnect();break;
             case  5: cmd_exec();break;
             case  6: player_floors=n;break;
             case  7: player_fov_degrees=fixtof(n);break;
             case  8: player_fps=n;break;
             case  9: player_gamma=n;cmd_gamma();break;
             case 10: player_gfx_mode=n;cmd_gfx_mode();break;
             case 11: cmd_gfx_modes();break;
             case 12: player_heightfix=n;break;
             case 13: player_invert=n;break;
             case 14: light_con=n;break;
             case 15: light_depth=fixtof(n);break;
             case 16: cmd_loopdemo();break;
             case 17: cmd_map();break;
             case 18: cmd_mapinfo();break;
             case 19: player_map_rotate=n;break;
             case 20: player_map=n;break;
             case 21: player_map_size=n;break;
             case 22: cmd_mem();break;
             case 23: internal_setstring(player_pic_con,parse_words[1]);
                      cmd_pic_con();break;
             case 24: cmd_playdemo();break;
             case 25: cmd_pwad();break;
             case 26: cmd_quit();break;
             case 27: cmd_recdemo();break;
             case 28: player_r_grad=n;break;
             case 29: player_r_gun=n;break;
             case 30: cmd_stop();break;
             case 31: cmd_syntax();break;
             case 32: cmd_time();break;
             case 33: cmd_timerefresh();break;
             case 34: player_trace=1;break;
             case 35: cmd_viewpic();break;
             case 36: cmd_viewtex();break;
             case 37: player_vsync=n;break;
             case 38: cmd_wads();break;
             case 39: player_walls=n;break;
             case 40: player_windowsize=n;break;
         }
    }

    // Return the error value. DEBUG: Do we use this?
    return(error);
}

void exec_command(char *q){
    // Processes a given console command line.
    strcpy(parse_line,q);
    con_parse();
    exec_parsed();
}



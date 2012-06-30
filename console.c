// FILE         : CONSOLE.C
// COMMENT      : Conains console routines and general parsing code.
// DATE STARTED : 07 MARCH 1999.

// INCLUDES:

#include "djdoom.h"
#include "console.h"
#include "player.h"
#include "cmds.h"
#include "gfx.h"
#include "wad_io.h"

// FUNCTIONS:

void con_parse(){
    // A general parsing routine, not specific to the console. Extracts words
    // from PARSE_LINE, and puts them into lower case in PARSE_WORDS. Also
    // sends PARSE_NWORDS to the number of words found.
    int i,j,on_space,on_quote;
    char ch;

    // Initiate.
    parse_nwords=0;

    // Make sure that we have something to parse =) ...
    if(parse_line[0]!=0){

        // Set up the flags.
        on_space=0;
        on_quote=0;
        if(parse_line[0]==32){on_space=1;}
        if(parse_line[0]==34){on_quote=1;}
        j=0;

        // Extract the words. We can check for words by the fact that they are
        // seperated by blocks of spaces (unless we're in between quotes).
        for(i=0;i<=strlen(parse_line);i++){
            ch=parse_line[i];

            // Add the letter to the word/quote.
            if(on_quote==0){
                if(ch!=32&&ch!=34&&ch!=0){
                     parse_words[parse_nwords][j++]=ch;
                }
            }
            else{
                if(ch!=34&&ch!=0){
                     parse_words[parse_nwords][j++]=ch;
                }
            }

            // Check for the start of a word/quote.
            if(ch!=32&&on_space==1){
                on_space=0;
                if(ch==34){
                    on_quote=1;
                    ch=65; // Just a dummy to ignore rest of the quote code.
                }
            }

            // Check for the end of the word/quote.
            if(on_quote==0){
                if((ch==32||ch==0)&&on_space==0&&j!=0){
                    parse_words[parse_nwords][j]=0;
                    parse_nwords++;
                    on_space=1;
                    j=0;
                }
            }
            else{
                if((ch==34||ch==0)&&i!=0){
                    parse_words[parse_nwords][j]=0;
                    parse_nwords++;
                    on_space=1;
                    on_quote=0;
                    j=0;
                }
            }
        }

        // Put the words into lower case, and ignore comments.
        for(i=0;i<parse_nwords;i++){
            // Put into lower case.
            strlwr(parse_words[i]);

            // Check for a comment.
            if(parse_words[i][0]==47&&parse_words[i][1]==47){
                parse_nwords=i; // Reset the number of words, and this loop too.
            }
        }
    }
}

void con_scroll(){
    // Scrolls up the console.
    int x,y;

    // Do the scrolling ...
    for(y=0;y<max_conbuf_y-1;y++){
        for(x=0;x<max_conbuf_x-2;x++){
            conbuf[x][y]=conbuf[x][y+1];
        }
    }
}

void con_draw(){
    // Draws a scaled, lit picture of the console background.
    draw_scaled_wad_pic(conpic,screen_width,screen_height,light_con,0,-con_height,screen_height);
}

void con_letter(int x,int y,byte a){
    // Draws a letter onto the console.
    int dx,dy,z;
    byte *p=vgabuf->line[0];

    // Draw the letter in a 8 x 8 loop.
    for(dx=0;dx<8;dx++){
        for(dy=0;dy<8;dy++){
	    z=quake_font[(a<<6)+(dy<<3)+dx];
            // Draw the pixel, with the mask value of 0.
	    if(z!=0){
	        *(p+(y+dy)*screen_width+x+dx)=z;
            }
        }
    }
}

void con_printf(byte *q){
    // Prints out a string to the last output line of the console.
    int x;

    // Scroll up.
    con_scroll();

    // Clear the last line.
    for(x=0;x<max_conbuf_x;x++){
        conbuf[x][max_conbuf_y-1]=32;
    }

    // Print each letter at a time.
    x=0;
    if(*q!=0){
        do{
            conbuf[x++][max_conbuf_y-1]=*q++;
        }while(*q!=0);
    }
}

void con_strnice(char *q){
    if(*q!=0){
        do{
            *q+=128;
            q++;
        }while(*q!=0);
    }
}

void con_refresh(){
    // This draws the console text from CONBUF.
    int x,y,max_x,max_y,ix,iy;
    int len,i,j,k;

    // Draw the background picture.
    con_draw();

    // Initiate.
    max_x=((screen_width)-(16*screen_width/320))/8-1;
    ix=7*screen_width/320;
    max_y=(screen_height-con_height)/8-(screen_height/200)-1;
    iy=(screen_height-con_height)-(8*screen_height/200)-8-(max_y*8);
    if(con_height==0){
        max_y--;
        iy+=8;
    }

    // Refresh all the letters except for the last line.
    for(y=0;y<max_y;y++){
        for(x=0;x<=max_x;x++){
            con_letter(x*8+ix,y*8+iy,conbuf[x][y+max_conbuf_y-max_y]);
        }
    }

    // Refresh the last line (the input line).
    len=strlen(conget);
    i=0;
    j=len;
    k=1;
    if(j>max_x-2){
        j=max_x-1;
        i=len-j;
        k=0;
    }
    if(k==1){
        con_letter(ix,max_y*8+iy,']');
    }
    for(x=0;x<j;x++){
        con_letter((x+k)*8+ix,max_y*8+iy,conget[x+i]);
    }
    if((cycles/35)%2==0){   // Make a variable, CURSOR_SPEED =)
        con_letter((j-(1-k))*8+8+ix,max_y*8+iy,11);
    }
}

void con_bringdown(){
    // This routine is called when the console key is pressed during play.

    // Initiate.
    player_con=1;
    conget[0]=0;
    con_height=screen_height/2;

    // Clear the Keyboard buffer.
    do{
        readkey();
    }while(keypressed()==TRUE);

    // Debug ... the following is *** TEMPORARY *** fix up code to make sure
    // that holding down on the console key doesn't bring it back up.
    // Note that the game pauses when the console key is being held down!
    if(key[KEY_TILDE]!=0){
        do{}while(key[KEY_TILDE]!=0);
    }
}

void con_replace(){
    // Completes the command for you, and replaces CONGET.
    int i,got,best=1000,match;

    // Find the best match in the list of commands.
    for(i=0;i<max_cmds;i++){
        match=bcmp(con_cmds[i].name,conget,strlen(conget));
        if(match<best){
            best=match;
            got=i;
        }
    }

    // Replace the string, only if we have some sort of match.
    if(con_cmds[got].name[0]==conget[0]){
        sprintf(conget,"%s",con_cmds[got].name); // Should be /%s really :)
    }
}

void con_lastcommand(){
    // Scrolls you up in the list of last commands.
    int i; // <---- ???

    // Replace the input string with the last command.
    //strcpy(conget,"last command");
}

void con_react(){
    // This routine is called instead of REACT() when the console is down.
    int n,r,scan;
    char ch;

    // Initiate.
    n=strlen(conget);

    // If there is a key waiting in the buffer ...
    if(keypressed()==TRUE){

        // ... then find out what it is.
        r=readkey();
        ch=r&0xff;
        scan=r>>8;

        // Check for normal keyboard input.
        if(n<max_conbuf_x-4){
            if(ch!=96&&ch!=13&&ch!=27&&ch!=8&&ch!=9){
                conget[n]=ch;
                conget[n+1]=0;
            }
        }

        // Check for the tab key.
        if(ch==9&&n>0){
            // Replace the console input string with a better one =)
            con_replace();
            n=strlen(conget);
        }

        // Check for the up arrow key.
        if(scan==KEY_UP){
            con_lastcommand();
            n=strlen(conget);
        }

        // Check for the delete key.
        if(ch==8&&n>0){
            conget[n-1]=0;
        }

        // Check for the enter key.
        if(ch==13){
            sprintf(saybuf,"]%s",conget);
            con_printf(saybuf);
            exec_command(conget);
            conget[0]=0;
        }
    }

    // Check for the console key.
    if(key[KEY_TILDE]!=0){
        do{}while(key[KEY_TILDE]!=0);
        player_con=0;
    }
}

void con_init(){
    // Initiates the console (not the memory part though).
    int error;

    // Decide on what background picture to use.
    strcpy(player_pic_con,"titlepic");
    if(strcmp(wad_path[0],"doom2.wad")==0){
        strcpy(player_pic_con,"victory2");
    }

    // Load the background console picture.
    conpic=wad_loadpic(INTERNAL_WAD,player_pic_con);
    if(conpic==NULL){
        shutdown_error("Couldn't load %c%s%c",34,player_pic_con,34);
    }
}

void con_null(){
    // The disconnected console loop.

    // Initiate.
    con_height=0;

    // Keen looping until we are connected, or until we quit.
    do{
        // Do the (null) game cycles.
        if(cycles<gcounter){cycles=gcounter;}

        // React to then refresh the console.
        clear_to_color(vgabuf,0); // <-- Do we need to do this?
        con_react();
        con_refresh();
        if(player_vsync==1){vsync();}
        blit(vgabuf,screen,0,0,0,0,screen_width,screen_height);
    }while(player_game_over==0&&player_connected==0);
}


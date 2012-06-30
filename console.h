// FILE         : CONSOLE.H
// COMMENT      : Contains header data related to the console and to parsing.
// DATE STARTED : 19 MARCH 1999.

// Engine limits.

#define max_parse                    200
#define max_conbuf_x                 128
#define max_conbuf_y                  96

// Parsing data.

char parse_line[max_parse];                 // The string to parse.
char parse_words[max_parse][max_parse];     // Words extracted from the string.
int parse_nwords;                           // The number of words in the string.
int parse_nlines;                           // The script file line number.

// Global console data.

byte conbuf[max_conbuf_x][max_conbuf_y];    // The console text buffer.
byte conget[max_conbuf_x];                  // The console input buffer.
int con_height;                             // Height from the bottom of the screen.
int con_alive;                              // Has the console been loaded?

// The console background picture.
WAD_PIC *conpic;


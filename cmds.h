// FILE         : CMDS.H
// COMMENT      : Contains header data for the console commands.
// DATE STARTED : 03 APRIL 1999.

// Engine limits.

#define hard_max_cmds  100

// Console command struct.
// NOTE: The INTS in this struct may be used as FIXED were appropriate.

struct{
    byte type;         // C = command, F = FIXED,  I = INT, S = string.
    byte name[32];     // The name of the console command.
    byte s_mirror[32]; // A mirror to the text variable, if used.
    int mirror;        // A mirror of the variable the command represents, if used.
    int min;           // The lower bound on the variable.
    int max;           // The upper bound on the variable, or number of arguments.
}con_cmds[hard_max_cmds];

// Global command data.

int max_cmds;       // The actual number of console commands.


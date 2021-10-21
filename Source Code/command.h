// file:		command.h for Week 8
// purpose;		to separate a list of tokens into a sequence of commands.
// assumptions:		any two successive commands in the list of tokens are separated
//			by one of the following command separators:
//				"|"  - pipe to the next command
//				"&"  - shell does not wait for the proceeding command to terminate
//				";"  - shell waits for the proceeding command to terminate
// author:		HX
// date:		2006.09.21
// last modified:	2006.10.05
// note:		not tested therefore it may contain errors

#define MAX_NUM_COMMANDS  1000

// command separators
#define pipeSep  "|"
#define conSep   "&"
#define seqSep   ";"

struct CommandStruct {
   int first;      // index to the first token in the array "token" of the command
   int last;       // index to the first token in the array "token" of the command
   char *sep;	   // the command separator that follows the command,  must be one of "|", "&", and ";"
   char **argv;
   char *stdin_file;
   char *stdout_file;
};

typedef struct CommandStruct Command;  // command type

int separateCommands(char *token[], Command command[]);

// file:		command.c for Week 8
// purpose;		to separate a list of tokens into a sequence of commands.
// assumptions:		any two successive commands in the list of tokens are separated
//			by one of the following command separators:
//				"|"  - pipe to the next command
//				"&"  - shell does not wait for the proceeding command to terminate
//				";"  - shell waits for the proceeding command to terminate
// author:		HX
// date:		2006.09.21
// note:		not tested therefore it may contain errors

// updated: Travis Patriarca
// note: Allowed for wildcard using glob()

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glob.h>
#include "command.h"

void searchRedirection(char *token[], Command *cp)
{
    int i;
    for (i=cp->first; i<=cp->last; ++i) {
        if (strcmp(token[i], "<") == 0) {   // standard input redirection
             cp->stdin_file = token[i+1];
             ++i;
        } else if (strcmp(token[i], ">") == 0) { // standard output redirection
             cp->stdout_file = token[i+1];
             ++i;
        }
    }
}

// return 1 if the token is a command separator
// return 0 otherwise
int separator(char *token)
{
    int i=0;
    char *commandSeparators[] = {pipeSep, conSep, seqSep, NULL};

    while (commandSeparators[i] != NULL) {
        if (strcmp(commandSeparators[i], token) == 0) {
            return 1;
        }
        ++i;
    }

    return 0;
}

// fill one command structure with the details
void fillCommandStructure(Command *cp, int first, int last, char *sep)
{
     cp->first = first;
     cp->last = last - 1;
     cp->sep = sep;
}

// build command line argument vector for execvp function
void buildCommandArgumentArray(char *token[], Command *cp)
{
    glob_t globResult;

    int n = 0;
    for (int t=cp->first; t<=cp->last; ++t ) {
        if (strcmp(token[t], ">") != 0 && strcmp(token[t], "<") != 0) {
            glob(token[t], GLOB_NOCHECK, 0, &globResult);
            n += globResult.gl_pathc;
            globfree(&globResult);
        }
    }

    n += 1; //null terminated

     // re-allocate memory for argument vector
     cp->argv = (char **) realloc(cp->argv, sizeof(char *) * n);
     if (cp->argv == NULL) {
         perror("realloc");
         exit(1);
     }

     // build the argument vector
     int i;
     int k = 0;
     for (i=cp->first; i<= cp->last; ++i ) {
         if (strcmp(token[i], ">") != 0 && strcmp(token[i], "<") != 0) {
             for (int n=0; n<globResult.gl_pathc; n++) {
                 glob(token[i], GLOB_NOCHECK, 0, &globResult);
                 cp->argv[k] = malloc(sizeof(char) * (strlen(globResult.gl_pathv[n])+1));
                 strcpy(cp->argv[k], globResult.gl_pathv[n]);
                 globfree(&globResult);
                 ++k;
             }
         }
     }

     cp->argv[k] = NULL;
}

int separateCommands(char *token[], Command command[])
{
     int i;
     int nTokens;

     // find out the number of tokens
     i = 0;
     while (token[i] != NULL) ++i;
     nTokens = i;

     // if empty command line
     if (nTokens == 0)
          return 0;

     // check the first token
     if (separator(token[0]))
          return -3;

     // check last token, add ";" if necessary
     if (!separator(token[nTokens-1])) {
        token[nTokens] = seqSep;
        ++nTokens;
     }

     int first=0;   // points to the first tokens of a command
     int last;      // points to the last  tokens of a command
     char *sep;     // command separator at the end of a command
     int c = 0;         // command index
     for (i=0; i<nTokens; ++i) {
         last = i;
         if (separator(token[i])) {
             sep = token[i];
             if (first==last)  // two consecutive separators
                 return -2;
             fillCommandStructure(&(command[c]), first, last, sep);
             //searchRedirection(token, &(command[c]));
             ++c;
             first = i+1;
         }
     }

     // check the last token of the last command
     if (strcmp(token[last], pipeSep) == 0) { // last token is pipe separator
          return -4;
     }

     // calculate the number of commands
     int nCommands = c;

     // handle standard in/out redirection and build command line argument vector
     for (i=0; i<nCommands; ++i) {
         searchRedirection(token, &(command[i]));
         buildCommandArgumentArray(token, &(command[i]));
     }

     return nCommands;
}

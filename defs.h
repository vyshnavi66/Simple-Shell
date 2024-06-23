#ifndef DEFS_H
#define DEFS_H
#include <unistd.h>
#include <string.h> 

// define a structure that stores the instruction in a structured manner and return this from the parse function.
typedef struct instruction{
    char* name;
    char** arguments;
    int nArguments;
    int redirection;
    char* outfile;
}instruction;

extern void errorMessage();
#endif 
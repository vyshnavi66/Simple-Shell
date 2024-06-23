// This is the C file for batch mode of our shell.

#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include "builtIns.h"
#include "defs.h"

void batchMode(char* filename){
    // open a file to read
    FILE* file = fopen(filename, "r");
    if (file==NULL){
        errorMessage();
        return;
    }

    char* line = NULL;
    size_t lineBuffer = 0;
    ssize_t read;

    while ((read = getline(&line, &lineBuffer, file)!=-1))
    {
        instruction** instructionArr = parseFn(line);
        int i=0;
        while(instructionArr[i]!=NULL){
            checkAndExecInstruction(instructionArr[i]);
            i++;
        }
    }

    free(line);
    fclose(file);

    exit(0);
    return;
};
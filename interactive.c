// This is the C file for interactive mode of our shell.

#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include "builtIns.h"
#include "defs.h"

void interactiveMode(){
    while(1){
        printf("dash> ");
        // This stores the input line
        char* inputLine = NULL;
        // This stores the position of the allocated memory for inputLine
        size_t inputBuffer = 0;
        // This indicates the length of the input line. signed size variable is used so as to keep check
        // of the error. If -1 is achieved, this means we have failed to obtain the input.
        ssize_t linelen;

        while((linelen = getline(&inputLine, &inputBuffer, stdin)) > 0){
            // change to interactive mode functions.
            instruction** instructionArr = parseFn(inputLine);
            // remove printf and check the input. If it is exit, cd or path go to the default commands
            int i=0;
            while(instructionArr[i]!=NULL){
                checkAndExecInstruction(instructionArr[i]);
                i++;
            }
            // else go to the commands specified in the paths that will be specified.
            break;
        }
    }
    return;
}
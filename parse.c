// This is the c program to parse any string, both in interactive and batch mode,
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

instruction** parseFn(char* inpStr){
    // get the input from the user and parse the string to get the command and its arguments.

    // define the array of instructions and a counter for the number of instructions
    instruction** instructionArr = NULL;
    int instructionCount = 0;

    // Tokenize the input string using '\n' as the first separator
    char* cmdBuffer1;
    char* token1 = strtok_r(inpStr, "\n", &cmdBuffer1);

    // filername if output redirection.
    char* filename;
    int redirect = 0;

    while(token1){
        // Tokenize each line using '&' as the second separator
        char* token2;
        char* cmdBuffer2;
        token2 = strtok_r(token1, "&", &cmdBuffer2);

        while(token2){
            // Tokenize each command using ' ' as the third separator
            char* cmdBuffer3;
            char* token3 = strtok_r(token2, " ", &cmdBuffer3);

            if (token3){
                // Create a new instruction
                instruction* inst = (instruction*)malloc(sizeof(instruction));
                inst->name = (char*)malloc((strlen(token3) + 1) * sizeof(char));
                strcpy(inst->name, token3);
                inst->nArguments = 0;
                inst->arguments = NULL;
                inst->outfile = NULL;
                inst->redirection = 0;

                while (token3) {
                    if (inst->outfile && inst->redirection > 1){
                        errorMessage();
                        exit(1);
                    }
                    // Add command arguments
                    token3 = strtok_r(NULL, " ", &cmdBuffer3);
                    // add a condition where token is >
                    if (token3 && strcmp(token3, ">")==0){
                        
                        // code for output re-direction
                        token3 = strtok_r(NULL, " ", &cmdBuffer3);
                        if (token3 == NULL){
                            errorMessage();
                            exit(1);
                        }
                        inst->outfile = token3;
                        inst->redirection++;
                    }
                    else{
                        if (token3) {
                            inst->arguments = (char**)realloc(inst->arguments, (inst->nArguments + 1) * sizeof(char*));
                            inst->arguments[inst->nArguments] = (char*)malloc((strlen(token3) + 1) * sizeof(char));
                            strcpy(inst->arguments[inst->nArguments], token3);
                            inst->nArguments++;
                    }
                    }
                }

                // Add the instruction to the array
                instructionArr = (instruction**)realloc(instructionArr, (instructionCount + 1) * sizeof(instruction*));
                instructionArr[instructionCount] = inst;
                instructionCount++;
            }

            token2 = strtok_r(NULL, "&", &cmdBuffer2);
        }
        token1 = strtok_r(NULL, "\n", &cmdBuffer1);
    }
    // Add a NULL pointer at the end of the instruction array
    instructionArr = (instruction**)realloc(instructionArr, (instructionCount + 1) * sizeof(instruction*));
    instructionArr[instructionCount] = NULL;

    return instructionArr;
}
// this file is to define and check for built in functions. If not a built in function,
// it checks whether these files are available in paths specified.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "defs.h"

void errorMessage(){
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    return;
}

// declared as global vars
char* defaultPaths[] = {"/bin", "/usr/bin"};
char** paths=defaultPaths;
int nPaths = 2;

// list of built in commands
char* builtInInstructions[] = {"cd", "exit", "path"};

// function that takes in instruction pointer and returns an int
// that indicates the status of execution of this built in command.
int built_cd(instruction* inst){
    // if the cd command does not have a path, return null.
    if (inst->nArguments == 0){
        errorMessage();
    }
    // if it fails to change directory, return -1 else it returns 0.
    else if (chdir(inst->arguments[0])!=0){
            errorMessage();
        }
    // on successful completion, return 1.
    return 1;
}

// function that takes in instruction pointer and returns an int
// that indicates the status of execution of this built in command.
int built_exit(instruction* inst){
    // exit out of the shell.
    if (inst->nArguments > 0){
        errorMessage();
    }
    exit(0);
};

// function that takes in instruction pointer and returns a pointer to an array of strings
// that indicates the status of execution of this built in command.
char** built_path(instruction* inst, int* ptrPaths){
    // initialize an array that is of the size of the number of arguments
    // in the instruction struct. These are the number of paths that are 
    // to be used because everytime we set them from scratch.
    
    // initialize the array paths
    char** paths = (char**)malloc((inst->nArguments + 3) * sizeof(char*));
    paths[0] = (char*) malloc ((strlen("/bin") + 1)*sizeof(char));
    strcpy(paths[0], "/bin");
    paths[1] = (char*) malloc ((strlen("/usr/bin") + 1)*sizeof(char));
    strcpy(paths[1], "/usr/bin");

    for(int i=0; i < inst->nArguments; i++){
        // allocate memory for the string that will be input
        paths[i+2] = (char*) malloc ((strlen(inst->arguments[i]) + 1)*sizeof(char));
        // copy the argument to path
        strcpy(paths[i+2], inst->arguments[i]);
    }

    *ptrPaths = inst->nArguments+2;

    return paths;
};

void forkAndExec(char* checkPath, instruction* inst){
    // create an args array
    // +2 for checkpath and NULL
    char** args =  malloc((inst->nArguments + 2) * sizeof(char*)); 

    args[0] = (char*)malloc(strlen(checkPath) + 1);
    strcpy(args[0], checkPath);

    int i;
    for (i = 1; i <= inst->nArguments; i++) {
        args[i] = (char*)malloc(strlen(inst->arguments[i - 1]) + 1);
        strcpy(args[i], inst->arguments[i - 1]);
    }
    args[i] = NULL;

    // fork and exec
    pid_t pid, wpid;
    int status;

    // fork the process to create a child and parent process.
    pid = fork();
    // if pid = 0 that means we're working with the child process
    if (pid==0){
        if (execvp(checkPath, args) == -1){
            errorMessage();
        }
        exit(0);
    }

    // error in forking
    else if (pid < -1){
        errorMessage();
    }

    else{
        // commands for the parent process
        while (1)
        {   
            // wpid is the wait id
            // pid is the process id for the child process
            // status is the exit status of the child process
            // WUNTRACED indicates that the parent should also exit if the child has exited.
            wpid = waitpid(pid, &status, WUNTRACED);

            // if child process has exited abnormally or due to signal, exit the whilw loop.
            if (WIFEXITED(status) || WIFSIGNALED(status)){
                break;
            }
        }
    }

    return;
};

void checkAndExecInstruction(instruction* inst){
    // for redirection
    // Open the file for redirection and truncate it

    // by default set them to default path values

    // if the instruction is cd
    // comparing if the strings are equal
    // Store stderr, stdout to restore them later
    
    int original_stdout = dup(STDOUT_FILENO);
    int original_stderr = dup(STDERR_FILENO);

    if (inst->redirection) {
        int fd = open(inst->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd == -1) {
            errorMessage();
        }

        // Flush any buffered output to the terminal
        fflush(stdout);
        fflush(stderr);

        // Redirect standard output and standard error to the file
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);

        close(fd); // Close the file descriptor
    }

    if (strcmp(inst->name, "cd")==0){
        int res = built_cd(inst);
    }

    // to exit from the shell
    else if (strcmp(inst->name, "exit")==0){
        int res = built_exit(inst);
    }

    // add path to the existing paths
    else if (strcmp(inst->name, "path")==0){
        paths = built_path(inst, &nPaths);
        if(!paths){
            errorMessage();
        }
    }

    // except the aforementioned built in commands, now we use commands that
    // will be custom called by concatenating it to the mentioned paths.
    else{
        // check if the command exists in the mentioned paths
        // also, additionally check if the user has mentioned absolute path for the command.

        // create the command
        int checkpathLen = 0;
        int pathLen = 0;
        int found = 0;
        
        for(int i=0; i<nPaths; i++){
            checkpathLen += strlen(paths[i]);
            checkpathLen += strlen(inst->name);

            char* checkPath = malloc((checkpathLen+2)*sizeof(char));
            strcpy(checkPath, paths[i]);
            strcat(checkPath, "/");
            strcat(checkPath,inst->name);

            // check if it exists in path. If yes, break. Else see in the paths
            // mentioned by the user/ other default paths

            // path found
            if(access(checkPath, X_OK) == 0){
                found = 1;

                // call fork and exec
                forkAndExec(checkPath, inst);
                break;
            }
            // if it does concat path with command and send to execute
            // else show error
        }

        if(found==0){
            errorMessage();
        }
    }
    // Restore the original file descriptors
    dup2(original_stdout, STDOUT_FILENO);
    dup2(original_stderr, STDERR_FILENO);

    close(original_stdout); // Close the original stdout file descriptor
    close(original_stderr); // Close the original stderr file descriptor
    return;
};

#ifndef PARSE_H
#define PARSE_H
#include "defs.h"

// modify the return type to the array of struct that should be created later that contains the
// commands and the arguments.
instruction** parseFn(char*);
#endif
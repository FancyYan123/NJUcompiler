#ifndef __LEXLIB__
#define __LEXLIB__

#include <stdlib.h>
#include <string.h>
#include <assert.h>

//functions to transform Octimal and heximal expression to decimal integer
int oct_to_deci(char* str);
int hex_to_deci(char* str);
float exp_to_float(char* str);

#endif

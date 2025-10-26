#ifndef LITLIB_H
#define LITLIB_H

#include <stdlib.h>

int rand_num(int start, int end);

#ifdef LITLIB_IMPL
int rand_num(int start, int end) { return rand() % (start - end + 1) + start; }

#endif // LITLIB_IMPL

#endif // LITLIB_H

// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#ifndef __INC_EVAL_WEIGHTS__
#define __INC_EVAL_WEIGHTS__

#include "defs.h"

typedef struct {
    const char *variablename;
    int elements;
    int *readaddr;
} weights_t;

extern int numweights;
extern weights_t weights[];

void weights_init();
void weights_push(int32_t *w_mg, int32_t *w_eg);
void weights_pull(int32_t *w_mg, int32_t *w_eg);
void weights_export();

#endif

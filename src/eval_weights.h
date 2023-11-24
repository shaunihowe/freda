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
void weights_push(int *w);
void weights_pull(int *w);
void weights_export();
void weights_tune();
int weights_simulate(board_t *board, int alpha, int beta, int depth);
void weights_tune_findcandidate(FILE *data, int *base_genome, int *candidate_genome);
int weights_tune_match(FILE *data, int *best_genome, int *candidate_genome, int *best_genome_score, int *candidate_genome_score);
#endif

// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#ifndef __INC_HASH__ 
#define __INC_HASH__ 

#include "defs.h"

typedef struct {
	uint64_t hash;
	int16_t betascore;
	uint8_t depth;
	uint8_t alpha;
	move_t bestmove;
} hash_t;

// hash.c
extern uint64_t hash[8][64];
extern uint64_t hash_castle[16];
extern uint64_t hash_ep[8];
extern uint64_t hash_turn[2];

extern int hash_size;
extern uint64_t hash_mask;
extern hash_t *hash_table;

uint64_t hash_rand();
void hash_init();
void hash_malloc(int size_mb);

#endif


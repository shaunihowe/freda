// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#ifndef __INC_HASH__ 
#define __INC_HASH__ 

#include "defs.h"

// hash.c
extern uint64_t hash[8][64];
extern uint64_t hash_castle[16];
extern uint64_t hash_ep[8];
extern uint64_t hash_turn[2];
uint64_t hash_rand();
void hash_init();

#endif


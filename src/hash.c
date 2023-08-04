// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#include "defs.h"

uint64_t hash[8][64];
uint64_t hash_castle[16];
uint64_t hash_ep[8];
uint64_t hash_turn[2];

uint64_t hash_rand()
{
	int i;
	uint64_t r = 0;
	for (i = 0; i < 1024; ++i)
		r ^= ((uint64_t)rand()) << (i % 64);
	return r;
}

void hash_init()
{
	int sqr,p;
	srand(time(0));
	for (sqr=0;sqr<64;++sqr)
		for (p=WHITE;p<=KING;++p)
			hash[p][sqr] = hash_rand();
	for (sqr=0;sqr<16;++sqr)
		hash_castle[sqr] = hash_rand();
	for (sqr=0;sqr<8;++sqr)
		hash_ep[sqr] = hash_rand();
	hash_turn[WHITE] = hash_rand();
	hash_turn[BLACK] = hash_rand();
	return;
}

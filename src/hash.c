// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#include "defs.h"

uint64_t hash[8][64];
uint64_t hash_castle[16];
uint64_t hash_ep[8];
uint64_t hash_turn[2];

int hash_size = 0x100000;
uint64_t hash_mask = 0xFFFFF;
hash_t *hash_table = NULL;

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
	hash_malloc(64);
	return;
}

void hash_malloc(int size_mb)
{
	uint64_t size_bits;
	if (hash_table != NULL)
		free(hash_table);
	if (size_mb < 4)
		size_mb = 4;
	else if (size_mb > 1024)
		size_mb = 1024;
	size_bits = 1 << BIT_CLZ((uint64_t)size_mb);
	size_mb = (int)size_bits;
	hash_size = (size_mb * (1024 * 1024)) / sizeof(hash_t);
	hash_mask = (uint64_t)(hash_size - 1);
	hash_table = malloc(sizeof(hash_t) * hash_size);
	memset(hash_table, 0, sizeof(hash_t) * hash_size);
	return;
}

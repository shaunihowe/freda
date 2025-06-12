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

uint64_t hash_seed;

uint64_t hash_rand()
{
	int rot;
	uint64_t keys[8] = {0xfacc5f414e14dd83, 0xec4147d55893b745, 0x61afc36079090ade, 0x2808c6edba6d6b04, 0xd15ec9d62e69e632, 0xfac8fd8eaf507ce6, 0xb7ea08d5647db77f, 0xcc419f9c6b2256e9};
	for (int i = 0; i < 1024; ++i)
	{
		hash_seed ^= keys[i % 8] + i;
		rot = BIT_POPCOUNT(hash_seed);
		hash_seed = (hash_seed << rot) | (hash_seed >> (64 - rot));
	}
	return hash_seed;
}

void hash_init()
{
	int sqr,p;
	hash_seed = 0xe93b0127190b637d;
	for (sqr=0;sqr<64;++sqr)
		for (p=WHITE;p<=KING;++p)
			hash[p][sqr] = hash_rand();
	for (sqr=0;sqr<16;++sqr)
		hash_castle[sqr] = hash_rand();
	for (sqr=0;sqr<8;++sqr)
		hash_ep[sqr] = hash_rand();
	hash_turn[WHITE] = hash_rand();
	hash_turn[BLACK] = hash_rand();
	hash_malloc(2);
	return;
}

void hash_malloc(int size_mb)
{
	uint64_t size_bits;
	if (hash_table != NULL)
		free(hash_table);
	if (size_mb < 1)
		size_mb = 1;
	else if (size_mb > 256)
		size_mb = 256;
	size_bits = 1 << BIT_CLZ((uint64_t)size_mb);
	size_mb = (int)size_bits;
	hash_size = (size_mb * (1024 * 1024)) / sizeof(hash_t);
	hash_mask = (uint64_t)(hash_size - 1);
	hash_table = malloc(sizeof(hash_t) * hash_size);
	memset(hash_table, 0, sizeof(hash_t) * hash_size);
	return;
}

// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#ifndef __INC_SEARCH__ 
#define __INC_SEARCH__ 

#include "defs.h"

#ifndef HASHSIZE
	#define HASHSIZE		0x100000
	#define HASHMASK		((uint64_t)(HASHSIZE-1))
#endif

typedef struct {
	uint64_t hash;
	int16_t betascore;
	uint8_t depth;
	uint8_t alpha;
	move_t bestmove;
} hash_t;

typedef struct {
	pthread_t threadid;
	board_t board;
	int thinking;
	clock_t starttime;
	jmp_buf checkpoint;
	int depthreached;
	int extdepthreached;
	int qsdepthreached;
	int score;
	int nodes;
	move_t bestmove;
	move_t pondermove;
	int depth;
	int onpv;
	int pvl[MAXPLY];
	move_t pv[MAXPLY][MAXPLY];
	hash_t hashtable[HASHSIZE];
	int history[64][64];
} search_t;

extern const int search_moveorder_pieces[8];

void search_init(search_t *search);
void search_start(search_t *search, board_t *board);
void search_stop(search_t *search);

void *thinkmove(void *searchp);

int search_alphabeta(search_t *search, int alpha, int beta, int depth);
int search_qsearch(search_t *search, int alpha, int beta);

#endif


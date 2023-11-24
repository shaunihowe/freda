// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#ifndef __INC_SEARCH__ 
#define __INC_SEARCH__ 

#include "defs.h"

typedef struct {
	pthread_t threadid;
	board_t board;
	int thinking;
	clock_t starttime;
	int endtime_cs;
	jmp_buf checkpoint;
	int depthreached;
	int extdepthreached;
	int qsdepthreached;
	int score;
        scorebound_t scorebound;
	int nodes;
	int hashhits;
	move_t bestmove;
	move_t pondermove;
	int depth;
	int onpv;
	int pvl[MAXPLY];
	move_t pv[MAXPLY][MAXPLY];
	int history[64][64];
} search_t;

extern const int search_moveorder_pieces[8];

void search_init(search_t *search);
void search_start(search_t *search, board_t *board);
void search_stop(search_t *search);

void *thinkmove(void *searchp);

int search_alphabeta(search_t *search, int alpha, int beta, int depth);
int search_qsearch(search_t *search, int alpha, int beta);

int search_alphabeta_alphatest(search_t *search, int alpha, int depth);
int search_alphabeta_betatest(search_t *search, int beta, int depth);
int search_alphabeta_alphafirst(search_t *search, int alpha, int beta, int depth);
int search_alphabeta_betafirst(search_t *search, int alpha, int beta, int depth);

int search_rankmoves_withhash(search_t * search, move_t *movelist, int *movescore, int moves);
int search_rankmoves(search_t * search, move_t *movelist, int *movescore, int moves);
int search_rankmoves_internal(search_t *search, move_t *movelist, int *movescore, int moves);

#endif


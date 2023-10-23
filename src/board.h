// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#ifndef __INC_BOARD__ 
#define __INC_BOARD__ 

#include "defs.h"

extern const int cm[64];

typedef struct {
	gubbins_t gubbins;
	uint64_t bb[8];
	undomove_t movehistory[2000];
} board_t;

void board_init(board_t *board);
void board_sethash(board_t *board);
void board_printboard(board_t *board);
void board_setboard(board_t *board, const char *fenstring);
void board_printmove(const move_t *move);
int board_sprintmove(char *str, const move_t *move);
void board_domove(board_t *board, const move_t *move);
void board_undomove(board_t *board);
int board_checktest(const board_t *board, int side);
int board_attack(const board_t *board, int side, int sqr);
int board_validcastle(const board_t *board, const move_t *move);
int board_repeattest(const board_t *board);

int board_generatecaptures(board_t *board, move_t *movelist);
int board_generatemoves(board_t *board, move_t *movelist);
void board_addmove(board_t *board, move_t *move, uint8_t source, uint8_t destination, uint8_t promotion);
void board_nextmove(move_t *movelist, int *movescore, int moves, int next);

int board_qsearch(board_t *board, int alpha, int beta);

#endif

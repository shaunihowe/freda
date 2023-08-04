// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#ifndef __INC_DEFS__
#define __INC_DEFS__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <setjmp.h>
#include <pthread.h>

#include "api.h"

#define MAXPLY	64

// define data types for various bit widths
typedef unsigned int uint;

#define SQR(x,y)	((y*8)+x)

// bitboard/piece values
#define WHITE	0
#define BLACK	1
#define PAWN  2
#define BISHOP  3
#define KNIGHT  4
#define ROOK  5
#define QUEEN 6
#define KING  7

#define true	 1
#define false	0

// Castle Status Values
#define CASTLE_ILLEGAL		-1
#define CASTLE_WK	        1
#define CASTLE_WQ	        2
#define CASTLE_BK	        4
#define CASTLE_BQ	        8

typedef struct {
	uint8_t source;
	uint8_t destination;
	uint8_t capture;
	uint8_t promotion;
} move_t;

typedef struct {
	int turn;
	int nummoves;
	int numpieces;
	int wkpos;
	int bkpos;
	uint castle;
	uint ep;
	uint fifty;
	uint64_t hash;
} gubbins_t;

typedef struct {
	move_t move;
	gubbins_t gubbins;
	uint64_t bb[8];
} undomove_t;

#include "board.h"
#include "hash.h"
#include "bitboard.h"
#include "search.h"
#include "eval.h"
#include "eval_weights.h"

#endif

// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#ifndef __INC_EVAL__
#define __INC_EVAL__

#include "defs.h"

extern int eval_piece_value[8];
extern int eval_pawn_sqr[64];
extern int eval_pawn_passed_sqr[64];
extern int eval_bishop_sqr[64];
extern int eval_knight_sqr[64];
extern int eval_rook_sqr[64];
extern int eval_queen_sqr[64];
extern int eval_king_sqr_mg[64];
extern int eval_king_sqr_eg[64];
extern int eval_pawn_doubled_penalty;
extern int eval_pawn_isolated_penalty;
extern int eval_pawn_backward_penalty;
extern int eval_rook_semiopen;
extern int eval_rook_open;
extern int eval_turn;

extern const int eval_flip[64];
float eval_cptoprob(int evalcp);
int eval_probtocp(float evalf);

int eval_full(const board_t *board);

#endif

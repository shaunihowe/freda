// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#ifndef __INC_EVAL__
#define __INC_EVAL__

#include "defs.h"

extern int32_t eval_piece_value[8];
extern int32_t eval_pawn_sqr[64];
extern int32_t eval_pawn_passed[64];
extern int32_t eval_bishop_sqr[64];
extern int32_t eval_knight_sqr[64];
extern int32_t eval_rook_sqr[64];
extern int32_t eval_queen_sqr[64];
extern int32_t eval_king_sqr[64];
extern int32_t eval_pawn_doubled_penalty;
extern int32_t eval_pawn_isolated_penalty;
extern int32_t eval_bishop_both;
extern int32_t eval_rook_semiopen;
extern int32_t eval_rook_open;
extern int32_t eval_king_pawns[4];
extern int32_t eval_nomatemat;
extern int32_t eval_turn;

extern const int eval_flip[64];
float eval_cptoprob(int32_t evalcp);
int32_t eval_probtocp(float evalf);

int32_t eval_full(const board_t *board);

#endif

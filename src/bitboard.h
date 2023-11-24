// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#ifndef __INC_BITBOARD__ 
#define __INC_BITBOARD__ 

#include "defs.h"

#define BIT_TEST(bb,bit) (((bb) >> (bit)) & (uint64_t)1)
#define BIT_SET(bb,bit) (bb |= ((uint64_t)1) << (bit))
#define BIT_CLEAR(bb,bit) (bb &= ~(((uint64_t)1) << (bit)))
#define BIT_POPCOUNT(bb) __builtin_popcountll(bb)
#define BIT_CTZ(bb) (bb ? __builtin_ctzll(bb) : 64)
#define BIT_CLZ(bb) (bb ? (63 - __builtin_clzll(bb)) : 64)
int BIT_POPLSB(uint64_t *bb);

extern const uint64_t bb_pawn_attack_white[64];
extern const uint64_t bb_pawn_attack_black[64];
extern const uint64_t bb_pawn_passed_white[64];
extern const uint64_t bb_pawn_passed_black[64];
extern const uint64_t bb_pawn_isolated[64];
extern const uint64_t bb_knight_move[64];
extern const uint64_t bb_diag_ne[64];
extern const uint64_t bb_diag_se[64];
extern const uint64_t bb_diag_sw[64];
extern const uint64_t bb_diag_nw[64];
extern const int bb_move_ne[64];
extern const int bb_move_se[64];
extern const int bb_move_sw[64];
extern const int bb_move_nw[64];
extern const uint64_t bb_line_n[64];
extern const uint64_t bb_line_e[64];
extern const uint64_t bb_line_s[64];
extern const uint64_t bb_line_w[64];
extern const int bb_move_n[64];
extern const int bb_move_e[64];
extern const int bb_move_s[64];
extern const int bb_move_w[64];
extern const uint64_t bb_king_move[64];

extern const int board_rank[64];
extern const int board_file[64];

// bitboard.c
void bb_display(uint64_t bb);

extern const uint64_t bb_file_mask[64];
extern const uint64_t bb_rank_mask[64];
extern const uint64_t bb_diagdown_mask[64];
extern const uint64_t bb_diagup_mask[64];
extern const uint64_t bb_rank[8];
extern const uint64_t bb_file[8];
extern const uint64_t bb_lightsquares;
extern const uint64_t bb_darksquares;


#endif


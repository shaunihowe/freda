#ifndef __INC_EVAL_WEIGHTS_MG__
#define __INC_EVAL_WEIGHTS_MG__

// 467 Weights

int32_t eval_weights_mg[] = {
	// eval_piece_value
	   0,	   0,	  82,	 363,	 324,	 469,	 998,	10000,
	// eval_pawn_sqr
	   0,   0,   0,   0,   0,   0,   0,   0,
	 -25,   4, -12, -19, -13,  25,  36, -14,
	 -18,  -1,  -2,  -5,   6,   4,  30,  -6,
	 -19,   1,  -1,  16,  21,   8,  11, -18,
	 -11,  17,  13,  26,  26,  13,  17, -11,
	  -6,  19,  41,  48,  48,  41,  19,  -6,
	  48,  82,  89,  80,  80,  89,  82,  48,
	   0,   0,   0,   0,   0,   0,   0,   0,
	// eval_pawn_passed
	   0,   0,   0,   0,   0,   0,   0,   0,
	  11,  12,  11,  10,  10,  11,  12,  11,
	  12,  13,  12,  11,  11,  12,  13,  12,
	  13,  14,  13,  12,  12,  13,  14,  13,
	  14,  15,  14,  13,  13,  14,  15,  14,
	  15,  16,  15,  14,  14,  15,  16,  15,
	  16,  17,  16,  15,  15,  16,  17,  16,
	   0,   0,   0,   0,   0,   0,   0,   0,
	// eval_bishop_sqr
	 -35,  -8, -17, -23, -16, -15, -38, -25,
	  -2,  10,  10,  -4,   2,  14,  25,  -5,
	  -5,  10,  10,  10,  10,  20,  13,   3,
	 -10,   7,   9,  20,  26,   8,   5,  -2,
	  -9,   1,  13,  40,  29,  28,   3,  -7,
	 -13,  27,  36,  29,  29,  36,  27, -13,
	 -36,  11,  13,   3,   3,  13,  11, -36,
	 -22,  -1, -57, -31, -31, -57,  -1, -22,
	// eval_knight_sqr
	 -86, -14, -43, -22,  -9, -18, -12, -17,
	 -21, -37,   0,   9,  10,  25,  -5, -12,
	 -14,   4,  22,  21,  29,  26,  32,  -8,
	  -6,  14,  26,  24,  37,  29,  28,  -1,
	  -2,  25,  29,  58,  44,  70,  26,  24,
	   4,  66,  81,  75,  75,  81,  66,   4,
	 -34,  -7,  66,  35,  35,  66,  -7, -34,
	-112, -40, -49,  10,  10, -49, -40,-112,
	// eval_rook_sqr
	  -4,   1,  13,  27,  26,  18, -19, -10,
	 -26,  -1,  -5,   4,  11,  21,   7, -48,
	 -26,  -9,  -1,  -2,  14,  12,   8, -16,
	 -19, -10,   2,  11,  19,   6,  17,  -8,
	  -9,   3,  18,  34,  32,  41,   5,  -6,
	  16,  46,  42,  34,  34,  42,  46,  16,
	  42,  38,  66,  73,  73,  66,  38,  42,
	  44,  43,  29,  59,  59,  29,  43,  44,
	// eval_queen_sqr
	   2, -10,  -3,  14,  -7, -16, -21, -39,
	 -25,   0,  16,   8,  13,  19,   4,   5,
	  -7,   9,  -2,   5,   3,   9,  18,   9,
	  -1, -15,   0,  -1,   5,   4,   9,   3,
	 -17, -16,  -6,  -6,   6,  21,   5,   6,
	  23,  19,  34,  23,  23,  34,  19,  23,
	  18,   2,  28,   0,   0,  28,   2,  18,
	  10,  23,  35,  35,  35,  35,  23,  10,
	// eval_king_sqr
	  -9,  35,  12, -45,   7, -22,  25,  15,
	   4,   9,  -7, -53, -36, -13,  11,  10,
	 -13, -15, -22, -55, -55, -28, -16, -24,
	 -36, -36, -36, -56, -56, -36, -36, -36,
	 -37, -37, -37, -57, -57, -37, -37, -37,
	 -38, -38, -38, -58, -58, -38, -38, -38,
	 -39, -39, -39, -40, -40, -39, -39, -39,
	 -40, -40, -40, -40, -40, -40, -40, -40,
	// eval_pawn_doubled_penalty
	  10,
	// eval_pawn_isolated_penalty
	  10,
	// eval_bishop_both
	  15,
	// eval_rook_semiopen
	  10,
	// eval_rook_open
	  15,
	// eval_king_pawns
	 -15,	 0,	  5,	5,
	// eval_nomatemat
	  -250,
	// eval_turn
	  15,
	// end of eval_weights_mg[]
	   0
};

#endif

// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#include "defs.h"

#include <math.h>

int32_t eval_piece_value[8];
int32_t eval_pawn_sqr[64];
int32_t eval_pawn_passed[64];
int32_t eval_bishop_sqr[64];
int32_t eval_knight_sqr[64];
int32_t eval_rook_sqr[64];
int32_t eval_queen_sqr[64];
int32_t eval_king_sqr[64];
int32_t eval_pawn_doubled_penalty;
int32_t eval_pawn_isolated_penalty;
int32_t eval_bishop_both;
int32_t eval_rook_semiopen;
int32_t eval_rook_open;
int32_t eval_king_pawns[4];
int32_t eval_nomatemat;
int32_t eval_turn;

const int eval_flip[64] = {
	56,57,58,59,60,61,62,63,
	48,49,50,51,52,53,54,55,
	40,41,42,43,44,45,46,47,
	32,33,34,35,36,37,38,39,
	24,25,26,27,28,29,30,31,
	16,17,18,19,20,21,22,23,
	 8, 9,10,11,12,13,14,15,
	 0, 1, 2, 3, 4, 5, 6, 7
};

float eval_winstoprob(int played, int wins)
{
	return (float)wins / (float)played;
}

float eval_winstoucb(int played, int wins)
{
	return (float)played / ((float)wins / ((float)played - (float)wins));
}

float eval_cptoprob(int32_t evalcp)
{
	float tevalf;
	tevalf = pow(10,((float)evalcp) / 400.0f);
	return tevalf / (tevalf + 1.0f);
}

int32_t eval_probtocp(float evalf)
{
	float tevalf;
	tevalf = evalf / (1.0f - evalf);
	return (int32_t)(log10f(tevalf) * 400.0f);
}

int32_t eval_full(const board_t *board)
{
	uint64_t bb, bb2;
	int sp, sqr;
	int matemat[2] = {0, 0};
	int32_t score[2] = {0, 0};
	int stage = 0;
	int32_t final = 0;
	// check king is on board for current turn
	if (!(board->bb[board->gubbins.turn] & board->bb[KING]))
		return -10000;

	// Find Mating Material for each side
	matemat[WHITE] = BIT_POPCOUNT(board->bb[WHITE] & (board->bb[BISHOP] | board->bb[KNIGHT])) > 1;
	matemat[WHITE] += (board->bb[WHITE] & (board->bb[PAWN] | board->bb[ROOK] | board->bb[QUEEN])) > 0;
	matemat[BLACK] = BIT_POPCOUNT(board->bb[BLACK] & (board->bb[BISHOP] | board->bb[KNIGHT])) > 1;
	matemat[BLACK] += (board->bb[BLACK] & (board->bb[PAWN] | board->bb[ROOK] | board->bb[QUEEN])) > 0;

	// check for no mate material draw
	if ( !matemat[WHITE] && !matemat[BLACK] )
		return 0;

	// Punish sides which have no mating material
	if (!matemat[WHITE])
		score[WHITE] += eval_nomatemat;
	if (!matemat[BLACK])
		score[BLACK] += eval_nomatemat;

	// White Pawns
	bb = board->bb[WHITE] & board->bb[PAWN];
	sp = BIT_POPCOUNT(bb);
	score[WHITE] += eval_piece_value[PAWN] * sp;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		score[WHITE] += eval_pawn_sqr[sqr];
		if (!(board->bb[BLACK] & board->bb[PAWN] & bb_pawn_passed_white[sqr]))
			score[WHITE] += eval_pawn_passed[sqr];
		if (board->bb[WHITE] & board->bb[PAWN] & bb_line_s[sqr])
			score[WHITE] -= eval_pawn_doubled_penalty;
		if (!(board->bb[WHITE] & board->bb[PAWN] & bb_pawn_isolated[sqr]))
			score[WHITE] -= eval_pawn_isolated_penalty;
	}
	// Black Pawns
	bb = board->bb[BLACK] & board->bb[PAWN];
	sp = BIT_POPCOUNT(bb);
	score[BLACK] += eval_piece_value[PAWN] * sp;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		score[BLACK] += eval_pawn_sqr[eval_flip[sqr]];
		if (!(board->bb[WHITE] & board->bb[PAWN] & bb_pawn_passed_black[sqr]))
			score[BLACK] += eval_pawn_passed[eval_flip[sqr]];
		if (board->bb[BLACK] & board->bb[PAWN] & bb_line_n[sqr])
			score[BLACK] -= eval_pawn_doubled_penalty;
		if (!(board->bb[BLACK] & board->bb[PAWN] & bb_pawn_isolated[sqr]))
			score[BLACK] -= eval_pawn_isolated_penalty;
	}
	// White Bishops
	bb = board->bb[WHITE] & board->bb[BISHOP];
	sp = BIT_POPCOUNT(bb);
	score[WHITE] += eval_piece_value[BISHOP] * sp;
	stage += sp;
	if ((bb & bb_lightsquares) && (bb & bb_darksquares))
		score[WHITE] += eval_bishop_both;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		score[WHITE] += eval_bishop_sqr[sqr];
	}
	// Black Bishops
	bb = board->bb[BLACK] & board->bb[BISHOP];
	sp = BIT_POPCOUNT(bb);
	score[BLACK] += eval_piece_value[BISHOP] * sp;
	stage += sp;
	if ((bb & bb_lightsquares) && (bb & bb_darksquares))
		score[BLACK] += eval_bishop_both;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		score[BLACK] += eval_bishop_sqr[eval_flip[sqr]];
	}
	// White Knights
	bb = board->bb[WHITE] & board->bb[KNIGHT];
	sp = BIT_POPCOUNT(bb);
	score[WHITE] += eval_piece_value[KNIGHT] * sp;
	stage += sp;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		score[WHITE] += eval_knight_sqr[sqr];
	}
	// Black Knights
	bb = board->bb[BLACK] & board->bb[KNIGHT];
	sp = BIT_POPCOUNT(bb);
	score[BLACK] += eval_piece_value[KNIGHT] * sp;
	stage += sp;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		score[BLACK] += eval_knight_sqr[eval_flip[sqr]];
	}
	// White Rooks
	bb = board->bb[WHITE] & board->bb[ROOK];
	sp = BIT_POPCOUNT(bb);
	score[WHITE] += eval_piece_value[ROOK] * sp;
	stage += sp * 2;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		score[WHITE] += eval_rook_sqr[sqr];
		bb2 = board->bb[PAWN] & bb_file_mask[sqr];
		if (!bb2)
			score[WHITE] += eval_rook_open;
		else if (!(bb2 & board->bb[WHITE]))
			score[WHITE] += eval_rook_semiopen;
	}
	// Black Rooks
	bb = board->bb[BLACK] & board->bb[ROOK];
	sp = BIT_POPCOUNT(bb);
	score[BLACK] += eval_piece_value[ROOK] * sp;
	stage += sp * 2;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		score[BLACK] += eval_rook_sqr[eval_flip[sqr]];
		bb2 = board->bb[PAWN] & bb_file_mask[sqr];
		if (!bb2)
			score[BLACK] += eval_rook_open;
		else if (!(bb2 & board->bb[BLACK]))
			score[BLACK] += eval_rook_semiopen;
	}
	// White Queens
	bb = board->bb[WHITE] & board->bb[QUEEN];
	sp = BIT_POPCOUNT(bb);
	score[WHITE] += eval_piece_value[QUEEN] * sp;
	stage += sp * 4;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		score[WHITE] += eval_queen_sqr[sqr];
	}
	// Black Queens
	bb = board->bb[BLACK] & board->bb[QUEEN];
	sp = BIT_POPCOUNT(bb);
	score[BLACK] += eval_piece_value[QUEEN] * sp;
	stage += sp * 4;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		score[BLACK] += eval_queen_sqr[eval_flip[sqr]];
	}
	// White King
	sp = BIT_POPCOUNT(board->bb[WHITE] & board->bb[PAWN] & bb_king_move[board->gubbins.wkpos]);
	if (sp > 3){sp = 3;}
	score[WHITE] += eval_king_pawns[sp];
	score[WHITE] += eval_king_sqr[board->gubbins.wkpos];
	// Black King
	sp = BIT_POPCOUNT(board->bb[BLACK] & board->bb[PAWN] & bb_king_move[board->gubbins.bkpos]);
	if (sp > 3){sp = 3;}
	score[BLACK] += eval_king_pawns[sp];
	score[BLACK] += eval_king_sqr[eval_flip[board->gubbins.bkpos]];

	final = score[WHITE] - score[BLACK];

	if (board->gubbins.turn == BLACK)
		final = -final;

	final += eval_turn;
	score[0] = (int16_t)final;
	score[1] = (final + 0x8000) >> 16;

	if (stage > 24)
		stage = 24;
	if (stage < 0)
		stage = 0;

	final = (score[0] * stage) +
		(score[1] * (24 - stage));
	final /= 24;

	// if we don't have mate material then we can't have a positive score.
	if (board->gubbins.turn == WHITE)
	{
		if ((final > -1) && (!matemat[WHITE]))
			final = -1;
		else if ((final < 1) && (!matemat[BLACK]))
			final = 1;
	}
	else if (board->gubbins.turn == BLACK)
	{
		if ((final > -1) && (!matemat[BLACK]))
			final = -1;
		else if ((final < 1) && (!matemat[WHITE]))
			final = 1;
	}

	// Never return a value of dead-on 0 (reserved for draws), give it +1 to the side to move.
	if (final == 0)
		final = 1;
	return final;
}

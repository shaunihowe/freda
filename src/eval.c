// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#include "defs.h"

#include <math.h>

int eval_piece_value[8];
int eval_pawn_sqr[64];
int eval_pawn_passed_sqr[64];
int eval_bishop_sqr[64];
int eval_knight_sqr[64];
int eval_rook_sqr[64];
int eval_queen_sqr[64];
int eval_king_sqr_mg[64];
int eval_king_sqr_eg[64];
int eval_pawn_doubled_penalty = 100;
int eval_turn = 25;

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

float eval_cptoprob(int evalcp)
{
	float tbasef, tevalf;
	tbasef = 2000.0f / 400.0f;
	tbasef = pow(10,tbasef);
	tevalf = (((float)evalcp) + 2000.0f) / 400.0f;
	tevalf = pow(10,tevalf);
	return tevalf / (tbasef + tevalf);
}

int eval_probtocp(float evalf)
{
	float tevalf;
	tevalf = 1.0f / ((1.0f - evalf) - 1.0f);
	tevalf = log10f(tevalf * 100000.0f);
	tevalf = (tevalf * 400.0f) - 2000.0f;
	return (int)tevalf;
}

int eval_full(const board_t *board)
{
	uint64_t bb;
	int sp, sqr;
	int matemat[2] = {0, 0};
	int numpawns[2] = {0, 0};
	int numminor[2] = {0, 0};
	int nummajor[2] = {0, 0};
	int mg[2] = {0, 0};
	int eg[2] = {0, 0};
	int stage[2] = {0, 0};
	int final = 0;
	// check king is on board for current turn
	if (!(board->bb[board->gubbins.turn] & board->bb[KING]))
		return -10000;

	// White Pawns
	bb = board->bb[WHITE] & board->bb[PAWN];
	numpawns[WHITE] = sp = BIT_POPCOUNT(bb);
	mg[WHITE] += eval_piece_value[PAWN] * sp;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		mg[WHITE] += eval_pawn_sqr[sqr];
		if (!(board->bb[BLACK] & board->bb[PAWN] & bb_pawn_passed_white[sqr]))
			mg[WHITE] += eval_pawn_passed_sqr[sqr];
		if (board->bb[WHITE] & board->bb[PAWN] & bb_line_s[sqr])
			mg[WHITE] -= eval_pawn_doubled_penalty;
	}
	// Black Pawns
	bb = board->bb[BLACK] & board->bb[PAWN];
	numpawns[BLACK] = sp = BIT_POPCOUNT(bb);
	mg[BLACK] += eval_piece_value[PAWN] * sp;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		mg[BLACK] += eval_pawn_sqr[eval_flip[sqr]];
		if (!(board->bb[WHITE] & board->bb[PAWN] & bb_pawn_passed_black[sqr]))
			mg[BLACK] += eval_pawn_passed_sqr[eval_flip[sqr]];
		if (board->bb[BLACK] & board->bb[PAWN] & bb_line_n[sqr])
			mg[BLACK] -= eval_pawn_doubled_penalty;
	}
	// White Bishops
	bb = board->bb[WHITE] & board->bb[BISHOP];
	numminor[WHITE] += sp = BIT_POPCOUNT(bb);
	mg[WHITE] += eval_piece_value[BISHOP] * sp;
	stage[WHITE] += sp;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		mg[WHITE] += eval_bishop_sqr[sqr];
	}
	// Black Bishops
	bb = board->bb[BLACK] & board->bb[BISHOP];
	numminor[BLACK] += sp = BIT_POPCOUNT(bb);
	mg[BLACK] += eval_piece_value[BISHOP] * sp;
	stage[BLACK] += sp;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		mg[BLACK] += eval_bishop_sqr[eval_flip[sqr]];
	}
	// White Knights
	bb = board->bb[WHITE] & board->bb[KNIGHT];
	numminor[WHITE] += sp = BIT_POPCOUNT(bb);
	mg[WHITE] += eval_piece_value[KNIGHT] * sp;
	stage[WHITE] += sp;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		mg[WHITE] += eval_knight_sqr[sqr];
	}
	// Black Knights
	bb = board->bb[BLACK] & board->bb[KNIGHT];
	numminor[BLACK] += sp = BIT_POPCOUNT(bb);
	mg[BLACK] += eval_piece_value[KNIGHT] * sp;
	stage[BLACK] += sp;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		mg[BLACK] += eval_knight_sqr[eval_flip[sqr]];
	}
	// White Rooks
	bb = board->bb[WHITE] & board->bb[ROOK];
	nummajor[WHITE] += sp = BIT_POPCOUNT(bb);
	mg[WHITE] += eval_piece_value[ROOK] * sp;
	stage[WHITE] += sp * 2;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		mg[WHITE] += eval_rook_sqr[sqr];
	}
	// Black Rooks
	bb = board->bb[BLACK] & board->bb[ROOK];
	nummajor[BLACK] += sp = BIT_POPCOUNT(bb);
	mg[BLACK] += eval_piece_value[ROOK] * sp;
	stage[BLACK] += sp * 2;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		mg[BLACK] += eval_rook_sqr[eval_flip[sqr]];
	}
	// White Queens
	bb = board->bb[WHITE] & board->bb[QUEEN];
	nummajor[WHITE] += sp = BIT_POPCOUNT(bb);
	mg[WHITE] += eval_piece_value[QUEEN] * sp;
	stage[WHITE] += sp * 4;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		mg[WHITE] += eval_queen_sqr[sqr];
	}
	// Black Queens
	bb = board->bb[BLACK] & board->bb[QUEEN];
	nummajor[BLACK] += sp = BIT_POPCOUNT(bb);
	mg[BLACK] += eval_piece_value[QUEEN] * sp;
	stage[BLACK] += sp * 4;
	while (bb)
	{
		sqr = BIT_POPLSB(&bb);
		mg[BLACK] += eval_queen_sqr[eval_flip[sqr]];
	}
	eg[WHITE] = mg[WHITE];
	eg[BLACK] = mg[BLACK];
	// White King
	mg[WHITE] += eval_king_sqr_mg[board->gubbins.wkpos];
	eg[WHITE] += eval_king_sqr_eg[board->gubbins.wkpos];
	// Black King
	mg[BLACK] += eval_king_sqr_mg[eval_flip[board->gubbins.bkpos]];
	eg[BLACK] += eval_king_sqr_eg[eval_flip[board->gubbins.bkpos]];

	if ((numpawns[WHITE] > 0) || (nummajor[WHITE] > 0) || (numminor[WHITE] > 1))
		matemat[WHITE]++;
	if ((numpawns[BLACK] > 0) || (nummajor[BLACK] > 0) || (numminor[BLACK] > 1))
		matemat[BLACK]++;

	// check for no mate material draw
	if (!matemat[WHITE] && !matemat[BLACK])
		return 0;

	mg[WHITE] *= stage[WHITE] + (stage[BLACK] * 2);
	eg[WHITE] *= 36 - (stage[WHITE] + (stage[BLACK] * 2));
	mg[BLACK] *= stage[BLACK] + (stage[WHITE] * 2);
	eg[BLACK] *= 36 - (stage[BLACK] + (stage[WHITE] * 2));

	final = (mg[WHITE] + eg[WHITE]) / 36;
	final -= (mg[BLACK] + eg[BLACK]) / 36;

	if (!matemat[WHITE])
		final -= 2500;
	if (!matemat[BLACK])
		final += 2500;

	if (board->gubbins.turn == BLACK)
		final = -final;
	//final += board->gubbins.hash & 255;
	final += eval_turn;
	final /= 10;
	if (final == 0){final = 1;}
	return final;
}

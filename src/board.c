// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#include "defs.h"

int nodes;

const int cm[64] = {
	13,15,15,15,12,15,15,14,
	15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,
	 7,15,15,15, 3,15,15,11
};

void board_init(board_t *board)
{
	board_setboard(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");
	return;
}

void board_sethash(board_t *board)
{
	int sqr,p;
	uint64_t sp;
	board->gubbins.hash = hash_castle[board->gubbins.castle] ^ hash_ep[board->gubbins.ep];
	board->gubbins.hash ^= hash_turn[board->gubbins.turn];
	for (p=WHITE;p<=KING;p++)
	{
		sp = board->bb[p];
		while (sp)
		{
			sqr = BIT_POPLSB(&sp);
			board->gubbins.hash ^= hash[p][sqr];
		}
	}
	return;
}

void board_printboard(board_t *board)
{
	int x,y,sqr;
	int sco;
	float scof;
	
	printf("# Current Position\n");
	for (y=7;y>=0;y--)
	{
		printf("# ");
		for (x=0;x<8;x++)
		{
			sqr = (y * 8) + x;

			if (BIT_TEST(board->bb[WHITE],sqr)){printf("W");}
			else if (BIT_TEST(board->bb[BLACK],sqr)){printf("B");}
			else {printf(".");}

			if (BIT_TEST(board->bb[PAWN],sqr)){printf("P");}
			else if (BIT_TEST(board->bb[BISHOP],sqr)){printf("B");}
			else if (BIT_TEST(board->bb[KNIGHT],sqr)){printf("N");}
			else if (BIT_TEST(board->bb[ROOK],sqr)){printf("R");}
			else if (BIT_TEST(board->bb[QUEEN],sqr)){printf("Q");}
			else if (BIT_TEST(board->bb[KING],sqr)){printf("K");}
			else {printf(".");}
			printf(" ");
		}
		printf("\n");
	}
	if (board->gubbins.turn == WHITE)
		printf("# Whites Turn		");
	else 
		printf("# Blacks Turn		");
	printf("%i pieces on board\n",board->gubbins.numpieces);
	printf("# Hash: 0x%lx\n",board->gubbins.hash);
	printf("# Ep: %i, Castle: %i\n",board->gubbins.ep,board->gubbins.castle);
	if (board_checktest(board, board->gubbins.turn))
		printf("# Check!\n");
	printf("# Evaluations     CP  Prob\n");
	sco = board_qsearch(board,-10002,10002);scof = eval_cptoprob(sco) * 100.0f;
	printf("# Q Search:    %5i (%5.2f%%)\n",sco,scof);
	sco = eval_full(board);scof = eval_cptoprob(sco) * 100.0f;
	printf("# Static:      %5i (%5.2f%%)\n",sco,scof);
	fflush(stdout);
	return;
}

void board_setboard(board_t *board, const char *fenstring)
{
	int x, y, tmp, section, tmp2, whitespace;
	board->gubbins.turn = WHITE;
	board->gubbins.nummoves = 0;
	board->gubbins.castle = 0;
	board->gubbins.ep = false;
	board->gubbins.numpieces = 0; // increment as we find them
	board->gubbins.fifty = 0;
	for (tmp=WHITE;tmp<=KING;tmp++)
		board->bb[tmp] = 0x0;

	//setboard rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
	section = 1;
	x = 0;
	y = 7;
	for (tmp = 0; tmp < strlen(fenstring); tmp++)
	{
		if (section == 1)
		{ // in board layout section of string
			if (fenstring[tmp] == ' ')
				section=2; // goto turn section
			else if (fenstring[tmp] == '/')
			{
				y=y-1;
				x=0;
			}
			else if (fenstring[tmp] == 'p')
			{
				//square[squareinmemory(x,y)] = BLACKPAWN;
				BIT_SET(board->bb[BLACK],SQR(x,y));
				BIT_SET(board->bb[PAWN],SQR(x,y));
				board->gubbins.numpieces++;
				x++;
			}
			else if (fenstring[tmp] == 'b')
			{
				//square[squareinmemory(x,y)] = BLACKBISHOP;
				BIT_SET(board->bb[BLACK],SQR(x,y));
				BIT_SET(board->bb[BISHOP],SQR(x,y));
				board->gubbins.numpieces++;
				x++;
			}
			else if (fenstring[tmp] == 'n')
			{
				//square[squareinmemory(x,y)] = BLACKKNIGHT;
				BIT_SET(board->bb[BLACK],SQR(x,y));
				BIT_SET(board->bb[KNIGHT],SQR(x,y));
				board->gubbins.numpieces++;
				x++;
			}
			else if (fenstring[tmp] == 'r')
			{
				//square[squareinmemory(x,y)] = BLACKROOK;
				BIT_SET(board->bb[BLACK],SQR(x,y));
				BIT_SET(board->bb[ROOK],SQR(x,y));
				board->gubbins.numpieces++;
				x++;
			}
			else if (fenstring[tmp] == 'q')
			{
				//square[squareinmemory(x,y)] = BLACKQUEEN;
				BIT_SET(board->bb[BLACK],SQR(x,y));
				BIT_SET(board->bb[QUEEN],SQR(x,y));
				board->gubbins.numpieces++;
				x++;
			}
			else if (fenstring[tmp] == 'k')
			{
				//square[squareinmemory(x,y)] = BLACKKING;
				BIT_SET(board->bb[BLACK],SQR(x,y));
				BIT_SET(board->bb[KING],SQR(x,y));
				board->gubbins.numpieces++;
				board->gubbins.bkpos = SQR(x,y);
				x++;
			}
			else if (fenstring[tmp] == 'P')
			{
				//square[squareinmemory(x,y)] = WHITEPAWN;
				BIT_SET(board->bb[WHITE],SQR(x,y));
				BIT_SET(board->bb[PAWN],SQR(x,y));
				board->gubbins.numpieces++;
				x++;
			}
			else if (fenstring[tmp] == 'B')
			{
				//square[squareinmemory(x,y)] = WHITEBISHOP;
				BIT_SET(board->bb[WHITE],SQR(x,y));
				BIT_SET(board->bb[BISHOP],SQR(x,y));
				board->gubbins.numpieces++;
				x++;
			}
			else if (fenstring[tmp] == 'N')
			{
				//square[squareinmemory(x,y)] = WHITEKNIGHT;
				BIT_SET(board->bb[WHITE],SQR(x,y));
				BIT_SET(board->bb[KNIGHT],SQR(x,y));
				board->gubbins.numpieces++;
				x++;
			}
			else if (fenstring[tmp] == 'R')
			{
				//square[squareinmemory(x,y)] = WHITEROOK;
				BIT_SET(board->bb[WHITE],SQR(x,y));
				BIT_SET(board->bb[ROOK],SQR(x,y));
				board->gubbins.numpieces++;
				x++;
			}
			else if (fenstring[tmp] == 'Q')
			{
				//square[squareinmemory(x,y)] = WHITEQUEEN;
				BIT_SET(board->bb[WHITE],SQR(x,y));
				BIT_SET(board->bb[QUEEN],SQR(x,y));
				board->gubbins.numpieces++;
				x++;
			}
			else if (fenstring[tmp] == 'K')
			{
				//square[squareinmemory(x,y)] = WHITEKING;
				BIT_SET(board->bb[WHITE],SQR(x,y));
				BIT_SET(board->bb[KING],SQR(x,y));
				board->gubbins.numpieces++;
				board->gubbins.wkpos = SQR(x,y);
				x++;
			}
			else if ((fenstring[tmp] > '0')&&(fenstring[tmp] < '9'))
			{
				whitespace = fenstring[tmp] - 48;
				for (tmp2=1;tmp2<=whitespace;tmp2++)
					x++;
			}
		}
		else if (section == 2)
		{   // turn section of string
			if (fenstring[tmp] == ' ')
				section=3; // castling section of string
			else if (fenstring[tmp] == 'w')
				board->gubbins.turn = WHITE;
			else if (fenstring[tmp] == 'b')
				board->gubbins.turn = BLACK;
		}
		else if (section == 3)
		{   // castling section of string
			if (fenstring[tmp] == ' ')
				section=4; // en-passant section of string
			else if (fenstring[tmp] == 'K')
				board->gubbins.castle+=CASTLE_WK;
			else if (fenstring[tmp] == 'Q')
				board->gubbins.castle+=CASTLE_WQ;
			else if (fenstring[tmp] == 'k')
				board->gubbins.castle+=CASTLE_BK;
			else if (fenstring[tmp] == 'q')
				board->gubbins.castle+=CASTLE_BQ;
		}
		else if (section == 4)
		{  // en-passant section of string
			if (fenstring[tmp] == ' ')
				section=5; // done
			else if (fenstring[tmp] == 'a')
				board->gubbins.ep = 1;
			else if (fenstring[tmp] == 'b')
				board->gubbins.ep = 2;
			else if (fenstring[tmp] == 'c')
				board->gubbins.ep = 3;
			else if (fenstring[tmp] == 'd')
				board->gubbins.ep = 4;
			else if (fenstring[tmp] == 'e')
				board->gubbins.ep = 5;
			else if (fenstring[tmp] == 'f')
				board->gubbins.ep = 6;
			else if (fenstring[tmp] == 'g')
				board->gubbins.ep = 7;
			else if (fenstring[tmp] == 'h')
				board->gubbins.ep = 8;
		}
	}
	
	board_sethash(board);
	return;
}

void board_printmove(const move_t *move)
{
	char pro = 0;
	int srcfile, srcrank, desfile, desrank;
	srcfile = board_file[move->source];
	srcrank = board_rank[move->source];
	desfile = board_file[move->destination];
	desrank = board_rank[move->destination];
	if (move->promotion == KNIGHT)
		pro = 'n';
	else if (move->promotion == BISHOP)
		pro = 'b';
	else if (move->promotion == ROOK)
		pro = 'r';
	else if (move->promotion == QUEEN)
		pro = 'q';
	else
	{
		printf("%c%i%c%i ", srcfile+96, srcrank, desfile+96, desrank);
		return;
	}
	printf("%c%i%c%i%c ", srcfile+96, srcrank, desfile+96, desrank, pro);
	return;
}

int board_sprintmove(char *str, const move_t *move)
{
	char pro = 0;
	int srcfile, srcrank, desfile, desrank;
	srcfile = board_file[move->source];
	srcrank = board_rank[move->source];
	desfile = board_file[move->destination];
	desrank = board_rank[move->destination];
	if (move->promotion == KNIGHT)
		pro = 'n';
	else if (move->promotion == BISHOP)
		pro = 'b';
	else if (move->promotion == ROOK)
		pro = 'r';
	else if (move->promotion == QUEEN)
		pro = 'q';
	else
		return sprintf(str, "%c%i%c%i ", srcfile+96, srcrank, desfile+96, desrank);
	return sprintf(str, "%c%i%c%i%c ", srcfile+96, srcrank, desfile+96, desrank, pro);
}

void board_domove(board_t *board, const move_t *move)  // do a move
{
	undomove_t *umove;
	int turn = board->gubbins.turn;
	int opp = 1 - turn;
	umove = &board->movehistory[board->gubbins.nummoves];
	
	int srcpiece = 0;
	int despiece = 0;
	int epsqr = 0;

	if (BIT_TEST(board->bb[PAWN],move->source))
		srcpiece = PAWN;
	else if (BIT_TEST(board->bb[BISHOP],move->source))
		srcpiece = BISHOP;
	else if (BIT_TEST(board->bb[KNIGHT],move->source))
		srcpiece = KNIGHT;
	else if (BIT_TEST(board->bb[ROOK],move->source))
		srcpiece = ROOK;
	else if (BIT_TEST(board->bb[QUEEN],move->source))
		srcpiece = QUEEN;
	else if (BIT_TEST(board->bb[KING],move->source))
		srcpiece = KING;

	if (BIT_TEST(board->bb[PAWN],move->destination))
		despiece = PAWN;
	else if (BIT_TEST(board->bb[BISHOP],move->destination))
		despiece = BISHOP;
	else if (BIT_TEST(board->bb[KNIGHT],move->destination))
		despiece = KNIGHT;
	else if (BIT_TEST(board->bb[ROOK],move->destination))
		despiece = ROOK;
	else if (BIT_TEST(board->bb[QUEEN],move->destination))
		despiece = QUEEN;
	else if (BIT_TEST(board->bb[KING],move->destination))
		despiece = KING;

	umove->gubbins = board->gubbins;
	umove->bb[0] = board->bb[0];
	umove->bb[1] = board->bb[1];
	umove->bb[2] = board->bb[2];
	umove->bb[3] = board->bb[3];
	umove->bb[4] = board->bb[4];
	umove->bb[5] = board->bb[5];
	umove->bb[6] = board->bb[6];
	umove->bb[7] = board->bb[7];
	
	board->gubbins.hash ^= hash_castle[board->gubbins.castle];
	board->gubbins.hash ^= hash_ep[board->gubbins.ep];
	board->gubbins.hash ^= hash_turn[board->gubbins.turn];
	
	board->gubbins.fifty++;
	BIT_CLEAR(board->bb[turn],move->source);board->gubbins.hash ^= hash[turn][move->source];
	BIT_CLEAR(board->bb[srcpiece],move->source);board->gubbins.hash ^= hash[srcpiece][move->source];
	if (despiece != 0)
	{
		board->gubbins.fifty = 0;
		board->gubbins.numpieces--;
		BIT_CLEAR(board->bb[opp],move->destination);board->gubbins.hash ^= hash[opp][move->destination];
		BIT_CLEAR(board->bb[despiece],move->destination);board->gubbins.hash ^= hash[despiece][move->destination];
	}
	BIT_SET(board->bb[turn],move->destination);board->gubbins.hash ^= hash[turn][move->destination];
	BIT_SET(board->bb[srcpiece],move->destination);board->gubbins.hash ^= hash[srcpiece][move->destination];
	
	if (move->promotion != 0)
	{
		BIT_CLEAR(board->bb[srcpiece],move->destination);board->gubbins.hash ^= hash[srcpiece][move->destination];
		BIT_SET(board->bb[move->promotion],move->destination);board->gubbins.hash ^= hash[move->promotion][move->destination];
	}
	
	if (srcpiece == PAWN)
	{
		board->gubbins.fifty = 0;
		if (turn == WHITE)
		{
			if (board_rank[move->source] == 5)
				if (board_file[move->destination] == board->gubbins.ep)
				{
					// move was ep, take the pawn off the board
					board->gubbins.numpieces--;
					epsqr = SQR(board->gubbins.ep-1,4);
					BIT_CLEAR(board->bb[BLACK],epsqr);board->gubbins.hash ^= hash[BLACK][epsqr];
					BIT_CLEAR(board->bb[PAWN],epsqr);board->gubbins.hash ^= hash[PAWN][epsqr];
				}
			board->gubbins.ep = 0;			// reset ep
			if (board_rank[move->source] == 2)
				if (board_rank[move->destination] == 4)
					board->gubbins.ep = board_file[move->source];
		}
		if (turn == BLACK)
		{
			if (board_rank[move->source] == 4)
				if (board_file[move->destination] == board->gubbins.ep)
				{
					// move was ep, take the pawn off the board
					board->gubbins.numpieces--;
					epsqr = SQR(board->gubbins.ep-1,3);
					BIT_CLEAR(board->bb[WHITE],epsqr);board->gubbins.hash ^= hash[WHITE][epsqr];
					BIT_CLEAR(board->bb[PAWN],epsqr);board->gubbins.hash ^= hash[PAWN][epsqr];
				}
			board->gubbins.ep = 0;				// reset ep
			if (board_rank[move->source] == 7)
				if (board_rank[move->destination] == 5)
					board->gubbins.ep = board_file[move->source];
		}
	}	
	else {board->gubbins.ep = 0;}

	if (srcpiece == KING)
	{
		if (turn == WHITE)
		{
			board->gubbins.wkpos = move->destination;
			if ((move->source == 4)&&(move->destination == 6))
			{
				BIT_CLEAR(board->bb[WHITE],7);board->gubbins.hash ^= hash[WHITE][7];
				BIT_CLEAR(board->bb[ROOK],7);board->gubbins.hash ^= hash[ROOK][7];
				BIT_SET(board->bb[WHITE],5);board->gubbins.hash ^= hash[WHITE][5];
				BIT_SET(board->bb[ROOK],5);board->gubbins.hash ^= hash[ROOK][5];
			}
			else if ((move->source == 4)&&(move->destination == 2))
			{
				BIT_CLEAR(board->bb[WHITE],0);board->gubbins.hash ^= hash[WHITE][0];
				BIT_CLEAR(board->bb[ROOK],0);board->gubbins.hash ^= hash[ROOK][0];
				BIT_SET(board->bb[WHITE],3);board->gubbins.hash ^= hash[WHITE][3];
				BIT_SET(board->bb[ROOK],3);board->gubbins.hash ^= hash[ROOK][3];
			}
		}
		else
		{
			board->gubbins.bkpos = move->destination;
			if ((move->source == 60)&&(move->destination == 62))
			{
				BIT_CLEAR(board->bb[BLACK],63);board->gubbins.hash ^= hash[BLACK][63];
				BIT_CLEAR(board->bb[ROOK],63);board->gubbins.hash ^= hash[ROOK][63];
				BIT_SET(board->bb[BLACK],61);board->gubbins.hash ^= hash[BLACK][61];
				BIT_SET(board->bb[ROOK],61);board->gubbins.hash ^= hash[ROOK][61];
			}
			else if ((move->source == 60)&&(move->destination == 58))
			{
				BIT_CLEAR(board->bb[BLACK],56);board->gubbins.hash ^= hash[BLACK][56];
				BIT_CLEAR(board->bb[ROOK],56);board->gubbins.hash ^= hash[ROOK][56];
				BIT_SET(board->bb[BLACK],59);board->gubbins.hash ^= hash[BLACK][59];
				BIT_SET(board->bb[ROOK],59);board->gubbins.hash ^= hash[ROOK][59];
			}
		}
	}

	board->gubbins.castle &= cm[move->source] & cm[move->destination];
	board->gubbins.turn = 1 - board->gubbins.turn;
	board->gubbins.hash ^= hash_turn[board->gubbins.turn];
	board->gubbins.hash ^= hash_castle[board->gubbins.castle];
	board->gubbins.hash ^= hash_ep[board->gubbins.ep];
	board->gubbins.nummoves++;
	return;
}

void board_undomove(board_t *board)
{
	undomove_t *umove;
	if (board->gubbins.nummoves<1){return;}
	board->gubbins.nummoves--;
	umove = &board->movehistory[board->gubbins.nummoves];
	board->gubbins = umove->gubbins;
	board->bb[0] = umove->bb[0];
	board->bb[1] = umove->bb[1];
	board->bb[2] = umove->bb[2];
	board->bb[3] = umove->bb[3];
	board->bb[4] = umove->bb[4];
	board->bb[5] = umove->bb[5];
	board->bb[6] = umove->bb[6];
	board->bb[7] = umove->bb[7];
	return;
}

int board_checktest(const board_t *board, int side)
{
	if (side == WHITE)
		return board_attack(board, BLACK, board->gubbins.wkpos);
	else
		return board_attack(board, WHITE, board->gubbins.bkpos);
	return 0;
}

int board_attack(const board_t *board, int side, int sqr)
{
	uint64_t pieces, attack_diag, attack_line, attack;
	pieces = board->bb[WHITE] | board->bb[BLACK];
	if (side == BLACK)
	{
		// is there a black pawn attacking?
		if (board->bb[BLACK] & board->bb[PAWN] & bb_pawn_attack_white[sqr])
			return 1;
		// is there a black knight attacking?
		else if (board->bb[BLACK] & board->bb[KNIGHT] & bb_knight_move[sqr])
			return 1;
		// is the black king attacking?
		else if (board->bb[BLACK] & board->bb[KING] & bb_king_move[sqr])
			return 1;
		// diagonal pieces?
		attack_diag = (board->bb[BISHOP] | board->bb[QUEEN]) & board->bb[BLACK];
		attack = attack_diag & bb_diag_ne[sqr];
		if (attack)
			if (BIT_CTZ(attack) == BIT_CTZ(pieces & bb_diag_ne[sqr]))
				return 1;
		attack = attack_diag & bb_diag_se[sqr];
		if (attack)
			if (BIT_CLZ(attack) == BIT_CLZ(pieces & bb_diag_se[sqr]))
				return 1;
		attack = attack_diag & bb_diag_sw[sqr];
		if (attack)
			if (BIT_CLZ(attack) == BIT_CLZ(pieces & bb_diag_sw[sqr]))
				return 1;
		attack = attack_diag & bb_diag_nw[sqr];
		if (attack)
			if (BIT_CTZ(attack) == BIT_CTZ(pieces & bb_diag_nw[sqr]))
				return 1;
		// line pieces?
		attack_line = (board->bb[ROOK] | board->bb[QUEEN]) & board->bb[BLACK];
		attack = attack_line & bb_line_n[sqr];
		if (attack)
			if (BIT_CTZ(attack) == BIT_CTZ(pieces & bb_line_n[sqr]))
				return 1;
		attack = attack_line & bb_line_e[sqr];
		if (attack)
			if (BIT_CTZ(attack) == BIT_CTZ(pieces & bb_line_e[sqr]))
				return 1;
		attack = attack_line & bb_line_s[sqr];
		if (attack)
			if (BIT_CLZ(attack) == BIT_CLZ(pieces & bb_line_s[sqr]))
				return 1;
		attack = attack_line & bb_line_w[sqr];
		if (attack)
			if (BIT_CLZ(attack) == BIT_CLZ(pieces & bb_line_w[sqr]))
				return 1;
	}
	else
	{
		// is there a white pawn attacking?
		if (board->bb[WHITE] & board->bb[PAWN] & bb_pawn_attack_black[sqr])
			return 1;
		// is there a white knight attacking?
		else if (board->bb[WHITE] & board->bb[KNIGHT] & bb_knight_move[sqr])
			return 1;
		// is the white king attacking?
		else if (board->bb[WHITE] & board->bb[KING] & bb_king_move[sqr])
			return 1;
		// diagonal pieces?
		attack_diag = (board->bb[BISHOP] | board->bb[QUEEN]) & board->bb[WHITE];
		attack = attack_diag & bb_diag_ne[sqr];
		if (attack)
			if (BIT_CTZ(attack) == BIT_CTZ(pieces & bb_diag_ne[sqr]))
				return 1;
		attack = attack_diag & bb_diag_se[sqr];
		if (attack)
			if (BIT_CLZ(attack) == BIT_CLZ(pieces & bb_diag_se[sqr]))
				return 1;
		attack = attack_diag & bb_diag_sw[sqr];
		if (attack)
			if (BIT_CLZ(attack) == BIT_CLZ(pieces & bb_diag_sw[sqr]))
				return 1;
		attack = attack_diag & bb_diag_nw[sqr];
		if (attack)
			if (BIT_CTZ(attack) == BIT_CTZ(pieces & bb_diag_nw[sqr]))
				return 1;
		// line pieces?
		attack_line = (board->bb[ROOK] | board->bb[QUEEN]) & board->bb[WHITE];
		attack = attack_line & bb_line_n[sqr];
		if (attack)
			if (BIT_CTZ(attack) == BIT_CTZ(pieces & bb_line_n[sqr]))
				return 1;
		attack = attack_line & bb_line_e[sqr];
		if (attack)
			if (BIT_CTZ(attack) == BIT_CTZ(pieces & bb_line_e[sqr]))
				return 1;
		attack = attack_line & bb_line_s[sqr];
		if (attack)
			if (BIT_CLZ(attack) == BIT_CLZ(pieces & bb_line_s[sqr]))
				return 1;
		attack = attack_line & bb_line_w[sqr];
		if (attack)
			if (BIT_CLZ(attack) == BIT_CLZ(pieces & bb_line_w[sqr]))
				return 1;
	}
	return 0;
}

int board_validcastle(const board_t *board, const move_t *move)
{
	int iscastle = 0;
	// Test if a castling move and is valid
	if (BIT_TEST(board->bb[KING], move->source))
	{
		if (move->source == 4)
		{
			if (move->destination == 6)
				iscastle = CASTLE_WK;
			else if (move->destination == 2)
				iscastle = CASTLE_WQ;
		}
		else if (move->source == 60)
		{
			if (move->destination == 62)
				iscastle = CASTLE_BK;
			else if (move->destination == 58)
				iscastle = CASTLE_BQ;
		}
		// Test none of the releveant castling squares are being attacked (illegal move!)
		if (iscastle == CASTLE_WK)
		{
			if (board_attack(board, BLACK, 5))
				return CASTLE_ILLEGAL;
			if (board_attack(board, BLACK, 6))
				return CASTLE_ILLEGAL;
		}
		else if (iscastle == CASTLE_WQ)
		{
			if (board_attack(board, BLACK, 3))
				return CASTLE_ILLEGAL;
			if (board_attack(board, BLACK, 2))
				return CASTLE_ILLEGAL;
		}
		else if (iscastle == CASTLE_BK)
		{
			if (board_attack(board, WHITE, 61))
				return CASTLE_ILLEGAL;
			if (board_attack(board, WHITE, 62))
				return CASTLE_ILLEGAL;
		}
		else if (iscastle == CASTLE_BQ)
		{
			if (board_attack(board, WHITE, 59))
				return CASTLE_ILLEGAL;
			if (board_attack(board, WHITE, 58))
				return CASTLE_ILLEGAL;
		}
	}
	return iscastle;
}

int board_repeattest(const board_t *board)
{
	int imove, repeats = 0;
	for (imove = board->gubbins.nummoves - board->gubbins.fifty; imove < board->gubbins.nummoves; ++imove)
		if (board->movehistory[imove].gubbins.hash == board->gubbins.hash)
			repeats++;
	return repeats;
}

int board_generatecaptures(board_t *board, move_t *movelist)
{
	uint64_t side, opp, attack;
	uint64_t pieces, sp, dp;
	int src, des, moves = 0;
	side = board->gubbins.turn;
	opp = BLACK - side;
	pieces = board->bb[WHITE] | board->bb[BLACK];
	// pawns
	if (side == WHITE)
	{
		sp = board->bb[WHITE] & board->bb[PAWN];
		while (sp)
		{
			src = BIT_POPLSB(&sp);
			dp = board->bb[BLACK] & bb_pawn_attack_white[src];
			while (dp)
			{
				des = BIT_POPLSB(&dp);
				if (board_rank[src] == 7)
				{
					board_addmove(board, &movelist[moves++], src, des, BISHOP);
					board_addmove(board, &movelist[moves++], src, des, KNIGHT);
					board_addmove(board, &movelist[moves++], src, des, ROOK);
					board_addmove(board, &movelist[moves++], src, des, QUEEN);
				}
				else
					board_addmove(board, &movelist[moves++], src, des, 0);
			}
			if (board_rank[src] == 7)
			{
				des = bb_move_n[src];
				if (!BIT_TEST(pieces,des))
				{
					board_addmove(board, &movelist[moves++], src, des, BISHOP);
					board_addmove(board, &movelist[moves++], src, des, KNIGHT);
					board_addmove(board, &movelist[moves++], src, des, ROOK);
					board_addmove(board, &movelist[moves++], src, des, QUEEN);
				}
			}
			else if (board_rank[src] == 6)
			{
				des = bb_move_n[src];
				if (!BIT_TEST(pieces,des))
				{
					board_addmove(board, &movelist[moves++], src, des, 0);
				}
			}
			else if ((board_rank[src] == 5)&&(board->gubbins.ep))
			{
				if (board->gubbins.ep == board_file[src] - 1)
				{
					des = bb_move_nw[src];
					board_addmove(board, &movelist[moves++], src, des, 0);
				}
				else if (board->gubbins.ep == board_file[src] + 1)
				{
					des = bb_move_ne[src];
					board_addmove(board, &movelist[moves++], src, des, 0);
				}
			}
		}
	}
	else if (side == BLACK)
	{
		sp = board->bb[BLACK] & board->bb[PAWN];
		while (sp)
		{
			src = BIT_POPLSB(&sp);
			dp = board->bb[WHITE] & bb_pawn_attack_black[src];
			while (dp)
			{
				des = BIT_POPLSB(&dp);
				if (board_rank[src] == 2)
				{
					board_addmove(board, &movelist[moves++], src, des, BISHOP);
					board_addmove(board, &movelist[moves++], src, des, KNIGHT);
					board_addmove(board, &movelist[moves++], src, des, ROOK);
					board_addmove(board, &movelist[moves++], src, des, QUEEN);
				}
				else
					board_addmove(board, &movelist[moves++], src, des, 0);
			}
			if (board_rank[src] == 2)
			{
				des = bb_move_s[src];
				if (!BIT_TEST(pieces,des))
				{
					board_addmove(board, &movelist[moves++], src, des, BISHOP);
					board_addmove(board, &movelist[moves++], src, des, KNIGHT);
					board_addmove(board, &movelist[moves++], src, des, ROOK);
					board_addmove(board, &movelist[moves++], src, des, QUEEN);
				}
			}
			else if (board_rank[src] == 3)
			{
				des = bb_move_s[src];
				if (!BIT_TEST(pieces,des))
				{
					board_addmove(board, &movelist[moves++], src, des, 0);
				}
			}
			else if ((board_rank[src] == 4)&&(board->gubbins.ep))
			{
				if (board->gubbins.ep == board_file[src] - 1)
				{
					des = bb_move_sw[src];
					board_addmove(board, &movelist[moves++], src, des, 0);
				}
				else if (board->gubbins.ep == board_file[src] + 1)
				{
					des = bb_move_se[src];
					board_addmove(board, &movelist[moves++], src, des, 0);
				}
			}
		}
	}
	// knights
	sp = board->bb[side] & board->bb[KNIGHT];
	while (sp)
	{
		src = BIT_POPLSB(&sp);
		dp = board->bb[opp] & bb_knight_move[src];
		while (dp)
		{
			des = BIT_POPLSB(&dp);
			board_addmove(board, &movelist[moves++], src, des, 0);
		}
	}
	// king
	sp = board->bb[side] & board->bb[KING];
	while (sp)
	{
		src = BIT_POPLSB(&sp);
		dp = board->bb[opp] & bb_king_move[src];
		while (dp)
		{
			des = BIT_POPLSB(&dp);
			board_addmove(board, &movelist[moves++], src, des, 0);
		}
	}
	// diagonal pieces
	sp = board->bb[side] & (board->bb[BISHOP] | board->bb[QUEEN]);
	while (sp)
	{
		src = BIT_POPLSB(&sp);
		// ne
		attack = board->bb[opp] & bb_diag_ne[src];
		if (attack)
			if (BIT_CTZ(attack) == BIT_CTZ(pieces & bb_diag_ne[src]))
			{
				des = BIT_CTZ(attack);
				board_addmove(board, &movelist[moves++], src, des, 0);
			}
		// se
		attack = board->bb[opp] & bb_diag_se[src];
		if (attack)
			if (BIT_CLZ(attack) == BIT_CLZ(pieces & bb_diag_se[src]))
			{
				des = BIT_CLZ(attack);
				board_addmove(board, &movelist[moves++], src, des, 0);
			}
		// sw
		attack = board->bb[opp] & bb_diag_sw[src];
		if (attack)
			if (BIT_CLZ(attack) == BIT_CLZ(pieces & bb_diag_sw[src]))
			{
				des = BIT_CLZ(attack);
				board_addmove(board, &movelist[moves++], src, des, 0);
			}
		// nw
		attack = board->bb[opp] & bb_diag_nw[src];
		if (attack)
			if (BIT_CTZ(attack) == BIT_CTZ(pieces & bb_diag_nw[src]))
			{
				des = BIT_CTZ(attack);
				board_addmove(board, &movelist[moves++], src, des, 0);
			}
	}
	// line pieces
	sp = board->bb[side] & (board->bb[ROOK] | board->bb[QUEEN]);
	while (sp)
	{
		src = BIT_POPLSB(&sp);
		// n
		attack = board->bb[opp] & bb_line_n[src];
		if (attack)
			if (BIT_CTZ(attack) == BIT_CTZ(pieces & bb_line_n[src]))
			{
				des = BIT_CTZ(attack);
				board_addmove(board, &movelist[moves++], src, des, 0);
			}
		// e
		attack = board->bb[opp] & bb_line_e[src];
		if (attack)
			if (BIT_CTZ(attack) == BIT_CTZ(pieces & bb_line_e[src]))
			{
				des = BIT_CTZ(attack);
				board_addmove(board, &movelist[moves++], src, des, 0);
			}
		// s
		attack = board->bb[opp] & bb_line_s[src];
		if (attack)
			if (BIT_CLZ(attack) == BIT_CLZ(pieces & bb_line_s[src]))
			{
				des = BIT_CLZ(attack);
				board_addmove(board, &movelist[moves++], src, des, 0);
			}
		// w
		attack = board->bb[opp] & bb_line_w[src];
		if (attack)
			if (BIT_CLZ(attack) == BIT_CLZ(pieces & bb_line_w[src]))
			{
				des = BIT_CLZ(attack);
				board_addmove(board, &movelist[moves++], src, des, 0);
			}
	}
	return moves;
}

int board_generatemoves(board_t *board, move_t *movelist)
{
	uint64_t side;
	uint64_t pieces, sp, dp;
	int src, des, moves = 0;
	side = board->gubbins.turn;
	pieces = board->bb[WHITE] | board->bb[BLACK];
	// pawns
	if (side == WHITE)
	{
		sp = board->bb[WHITE] & board->bb[PAWN];
		while (sp)
		{
			src = BIT_POPLSB(&sp);
			if (board_rank[src] < 6)
			{
				des = bb_move_n[src];
				if (!BIT_TEST(pieces,des))
				{
					board_addmove(board, &movelist[moves++], src, des, 0);
					if (board_rank[src] == 2)
					{
						des = bb_move_n[des];
						if (!BIT_TEST(pieces,des))
							board_addmove(board, &movelist[moves++], src, des, 0);
					}
				}
			}
		}
	}
	else if (side == BLACK)
	{
		sp = board->bb[BLACK] & board->bb[PAWN];
		while (sp)
		{
			src = BIT_POPLSB(&sp);
			if (board_rank[src] > 3)
			{
				des = bb_move_s[src];
				if (!BIT_TEST(pieces,des))
				{
					board_addmove(board, &movelist[moves++], src, des, 0);
					if (board_rank[src] == 7)
					{
						des = bb_move_s[des];
						if (!BIT_TEST(pieces,des))
							board_addmove(board, &movelist[moves++], src, des, 0);
					}
				}
			}
		}
	}
	// knights
	sp = board->bb[side] & board->bb[KNIGHT];
	while (sp)
	{
		src = BIT_POPLSB(&sp);
		dp = bb_knight_move[src];
		while (dp)
		{
			des = BIT_POPLSB(&dp);
			if (!BIT_TEST(pieces,des))
				board_addmove(board, &movelist[moves++], src, des, 0);
		}
	}
	// king
	sp = board->bb[side] & board->bb[KING];
	while (sp)
	{
		src = BIT_POPLSB(&sp);
		dp = bb_king_move[src];
		while (dp)
		{
			des = BIT_POPLSB(&dp);
			if (!BIT_TEST(pieces,des))
				board_addmove(board, &movelist[moves++], src, des, 0);
		}
		if ((src == 4)&&(side == WHITE))
		{
			if (board->gubbins.castle & CASTLE_WK)
				if (!(BIT_TEST(pieces, 5) || BIT_TEST(pieces, 6)))
					if (BIT_TEST(board->bb[WHITE] & board->bb[ROOK], 7))
						board_addmove(board, &movelist[moves++], 4, 6, 0);
			if (board->gubbins.castle & CASTLE_WQ)
				if (!(BIT_TEST(pieces, 1) || BIT_TEST(pieces, 2) || BIT_TEST(pieces, 3)))
					if (BIT_TEST(board->bb[WHITE] & board->bb[ROOK], 0))
						board_addmove(board, &movelist[moves++], 4, 2, 0);
		}
		else if ((src == 60)&&(side == BLACK))
		{
			if (board->gubbins.castle & CASTLE_BK)
				if (!(BIT_TEST(pieces, 61) || BIT_TEST(pieces, 62)))
					if (BIT_TEST(board->bb[BLACK] & board->bb[ROOK], 63))
						board_addmove(board, &movelist[moves++], 60, 62, 0);
			if (board->gubbins.castle & CASTLE_BQ)
				if (!(BIT_TEST(pieces, 57) || BIT_TEST(pieces, 58) || BIT_TEST(pieces, 59)))
					if (BIT_TEST(board->bb[BLACK] & board->bb[ROOK], 56))
						board_addmove(board, &movelist[moves++], 60, 58, 0);
		}
	}
	// diagonal pieces
	sp = board->bb[side] & (board->bb[BISHOP] | board->bb[QUEEN]);
	while (sp)
	{
		src = BIT_POPLSB(&sp);
		// ne
		dp = pieces & bb_diag_ne[src];
		if (dp)
			dp = dp | bb_diag_ne[BIT_CTZ(dp)];
		dp = dp ^ bb_diag_ne[src];
		while (dp)
		{
			des = BIT_POPLSB(&dp);
			board_addmove(board, &movelist[moves++], src, des, 0);
		}
		// se
		dp = pieces & bb_diag_se[src];
		if (dp)
			dp = dp | bb_diag_se[BIT_CLZ(dp)];
		dp = dp ^ bb_diag_se[src];
		while (dp)
		{
			des = BIT_POPLSB(&dp);
			board_addmove(board, &movelist[moves++], src, des, 0);
		}
		// sw
		dp = pieces & bb_diag_sw[src];
		if (dp)
			dp = dp | bb_diag_sw[BIT_CLZ(dp)];
		dp = dp ^ bb_diag_sw[src];
		while (dp)
		{
			des = BIT_POPLSB(&dp);
			board_addmove(board, &movelist[moves++], src, des, 0);
		}
		// nw
		dp = pieces & bb_diag_nw[src];
		if (dp)
			dp = dp | bb_diag_nw[BIT_CTZ(dp)];
		dp = dp ^ bb_diag_nw[src];
		while (dp)
		{
			des = BIT_POPLSB(&dp);
			board_addmove(board, &movelist[moves++], src, des, 0);
		}
	}
	// line pieces
	sp = board->bb[side] & (board->bb[ROOK] | board->bb[QUEEN]);
	while (sp)
	{
		src = BIT_POPLSB(&sp);
		// n
		dp = pieces & bb_line_n[src];
		if (dp)
			dp = dp | bb_line_n[BIT_CTZ(dp)];
		dp = dp ^ bb_line_n[src];
		while (dp)
		{
			des = BIT_POPLSB(&dp);
			board_addmove(board, &movelist[moves++], src, des, 0);
		}
		// e
		dp = pieces & bb_line_e[src];
		if (dp)
			dp = dp | bb_line_e[BIT_CTZ(dp)];
		dp = dp ^ bb_line_e[src];
		while (dp)
		{
			des = BIT_POPLSB(&dp);
			board_addmove(board, &movelist[moves++], src, des, 0);
		}
		// s
		dp = pieces & bb_line_s[src];
		if (dp)
			dp = dp | bb_line_s[BIT_CLZ(dp)];
		dp = dp ^ bb_line_s[src];
		while (dp)
		{
			des = BIT_POPLSB(&dp);
			board_addmove(board, &movelist[moves++], src, des, 0);
		}
		// w
		dp = pieces & bb_line_w[src];
		if (dp)
			dp = dp | bb_line_w[BIT_CLZ(dp)];
		dp = dp ^ bb_line_w[src];
		while (dp)
		{
			des = BIT_POPLSB(&dp);
			board_addmove(board, &movelist[moves++], src, des, 0);
		}
	}
	return moves;
}

void board_addmove(board_t *board, move_t *move, uint8_t source, uint8_t destination, uint8_t promotion)
{
	uint8_t capture = 0;
	if (BIT_TEST(board->bb[PAWN],destination)){capture = PAWN;}
	else if (BIT_TEST(board->bb[BISHOP],destination)){capture = BISHOP;}
	else if (BIT_TEST(board->bb[KNIGHT],destination)){capture = KNIGHT;}
	else if (BIT_TEST(board->bb[ROOK],destination)){capture = ROOK;}
	else if (BIT_TEST(board->bb[QUEEN],destination)){capture = QUEEN;}
	else if (BIT_TEST(board->bb[KING],destination)){capture = KING;}
	move->source = source;
	move->destination = destination;
	move->capture = capture;
	move->promotion = promotion;
	return;
}

void board_nextmove(move_t *movelist, int *movescore, int moves, int next)
{
	int cur, bst;
	move_t swp;
	long bstsco, swpsco;
	bst = next; bstsco = movescore[bst];
	for (cur=next; cur<moves; cur++)
	{
		if (movescore[cur] >= 100000)
		{
			bst = cur; bstsco = movescore[cur];
			break;
		}
		if (movescore[cur] > bstsco)
		{
			bst = cur; bstsco = movescore[cur];
		}
	}
	// swap them
	swp = movelist[next]; swpsco = movescore[next];
	movelist[next] = movelist[bst]; movescore[next] = movescore[bst];
	movelist[bst] = swp; movescore[bst] = swpsco;
	return;
}

void board_randomposition(board_t *board)
{
	int piece, colour, y, tmp, tmp2;
	int pawnrank[] = {0, 10, 9, 8, 7, 6, 5, 0};
	int rank[] = {8, 10, 9, 7, 6, 5, 4, 3};
	int tsqr, sqr[2], sqrscore[2], validsqr;
	board->gubbins.turn = rand() % 2;
	board->gubbins.nummoves = 0;
	board->gubbins.castle = 0;
	board->gubbins.ep = false;
	board->gubbins.numpieces = 0; // increment as we find them
	board->gubbins.fifty = 0;
	for (tmp = WHITE; tmp<=KING; tmp++)
		board->bb[tmp] = 0x0;

	for (piece = PAWN; piece <=KING; piece++)
		for (colour = WHITE; colour <= BLACK; colour++)
		{
			// how many of this piece is on the board
			if (piece == PAWN)
				tmp2 = (rand() % 6) + 2; // pawns between 2 and 7
			else if (piece == QUEEN)
				tmp2 = rand() % 2; // queens either 1 or none
			else if (piece == KING)
				tmp2 = 1; // king is always one
			else
				tmp2 = rand() % 3; // other pieces = 0, 1 or 2
			for (tmp = 1; tmp <= tmp2; tmp++)
			{
				// find an empty square
				do
				{
					// what rank to put it
					if (piece == PAWN)
					{
						sqr[0] = (rand() % 6) + 1;sqrscore[0] = pawnrank[sqr[0]];
						sqr[1] = (rand() % 6) + 1;sqrscore[1] = pawnrank[sqr[1]];
					}
					else
					{
						sqr[0] = rand() % 8;sqrscore[0] = rank[sqr[0]];
						sqr[1] = rand() % 8;sqrscore[1] = rank[sqr[1]];				
					}
					if (rand() % 16 == 0)
						y = sqr[rand() % 2];
					else if (sqrscore[0] > sqrscore[1])
						y = sqr[0];
					else
						y = sqr[1];
					if (colour == BLACK){y = 7 - y;}
					tsqr = SQR(rand() % 8, y);
					// test if we can out it here
					validsqr = 1;
					if (BIT_TEST(board->bb[WHITE] | board->bb[BLACK],tsqr))
						validsqr = 0;
					else if (board_attack(board, 1 - colour, tsqr) == 1)
						validsqr = 0;
					else if ((piece == BISHOP) && (tmp == 2))
					{
						// test if we are putting two bishops on same coloured squares
						y = BIT_CTZ(board->bb[colour] & board->bb[BISHOP]);
						if ((y % 2) == (tsqr % 2))
							validsqr = 0;
					}
				} while (validsqr == 0);
				// add the piece
				BIT_SET(board->bb[colour], tsqr);BIT_SET(board->bb[piece], tsqr);
				if ((piece == KING)&&(colour == WHITE))
					board->gubbins.wkpos = tsqr;
				else if ((piece == KING)&&(colour == BLACK))
					board->gubbins.bkpos = tsqr;
				board->gubbins.numpieces++;
			}
		}
	return;
}

int board_qsearch(board_t *board, int alpha, int beta)
{
	move_t movelist[128];
	int movescore[128];
	int moves, spscore, imove;

	spscore = eval_full(board);
	if (spscore >= beta)
		return beta;
	if (spscore > alpha)
		alpha = spscore;

	moves = board_generatecaptures(board, movelist);
	for (imove = 0; imove < moves; ++imove)
	{
		if (movelist[imove].capture == KING)
			return 10000 - board->gubbins.nummoves;
		movescore[imove] = search_moveorder_pieces[movelist[imove].capture] + search_moveorder_pieces[movelist[imove].promotion];
	}	

	for (imove = 0; imove < moves; ++imove)
	{
		board_nextmove(movelist, movescore, moves, imove);
		board_domove(board, &movelist[imove]);
		spscore = -board_qsearch(board, -beta, -alpha);
		board_undomove(board);
		if (spscore >= beta)
			return beta;
		if (spscore > alpha)
			alpha = spscore;
	}
	return alpha;
}


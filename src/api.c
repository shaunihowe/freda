// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#include "defs.h"


board_t mainboard;
search_t mainsearch;
search_output_t *mainsearch_output;

void api_init()
{
	api_stop();
	hash_init();
	weights_init();
	search_init(&mainsearch);
	api_setposition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq 0 1");

	/*printf("const uint64_t bb_pawn_isolated[64] = {\n");
	uint64_t bb;
	int sqr, file;
	for (int rank = 0; rank < 8; rank++)
	{
		bb = bb_file[0] | bb_file[1];
		BIT_CLEAR(bb, (rank * 8) + 0);
		printf("\t0x%lx,",bb);
		bb = bb_file[0] | bb_file[1] | bb_file[2];
		BIT_CLEAR(bb, (rank * 8) + 1);
		printf("0x%lx,",bb);
		bb = bb_file[1] | bb_file[2] | bb_file[3];
		BIT_CLEAR(bb, (rank * 8) + 2);
		printf("0x%lx,",bb);
		bb = bb_file[2] | bb_file[3] | bb_file[4];
		BIT_CLEAR(bb, (rank * 8) + 3);
		printf("0x%lx,",bb);
		bb = bb_file[3] | bb_file[4] | bb_file[5];
		BIT_CLEAR(bb, (rank * 8) + 4);
		printf("0x%lx,",bb);
		bb = bb_file[4] | bb_file[5] | bb_file[6];
		BIT_CLEAR(bb, (rank * 8) + 5);
		printf("0x%lx,",bb);
		bb = bb_file[5] | bb_file[6] | bb_file[7];
		BIT_CLEAR(bb, (rank * 8) + 6);
		printf("0x%lx,",bb);
		bb = bb_file[6] | bb_file[7];
		BIT_CLEAR(bb, (rank * 8) + 7);
		printf("0x%lx,\n",bb);
	}
	printf("};"); //*/

	return;
}

void api_setposition(const char *fenstring)
{
	api_stop();
	board_setboard(&mainboard, fenstring);
	return;
}

void api_getposition(char *fenstring)
{
	return;
}

void api_showposition()
{
	board_printboard(&mainboard);
	return;
}

game_status_t api_getstatus()
{
	if (mainboard.gubbins.turn == WHITE)
		return status_turn_white;
	else
		return status_turn_black;
}

void api_move(const char *move)
{
	move_t usermove;
	int pro = 0;
	if (move[4] == 'q'){pro=QUEEN;}
	if (move[4] == 'r'){pro=ROOK;}
	if (move[4] == 'n'){pro=KNIGHT;}
	if (move[4] == 'b'){pro=BISHOP;}
	usermove.source = ((move[1] - '1') * 8) + (move[0] - 'a');
	usermove.destination = ((move[3] - '1') * 8) + (move[2] - 'a');
	usermove.promotion = pro;
	usermove.capture = 0;
	api_stop();
	board_domove(&mainboard,&usermove);
	return;
}

void api_undomove()
{
	api_stop();
	board_undomove(&mainboard);
	return;
}

void api_go(search_output_t *search_output, clock_info_t *search_clock)
{
	int movesremaining;
	api_stop();
	mainsearch_output = search_output;
	if (mainboard.gubbins.turn == WHITE)
	{
		movesremaining = search_clock->white_remaining_moves;
		if (movesremaining == 0){movesremaining = 32;}
		mainsearch.endtime_cs = (search_clock->white_remaining_cs / movesremaining) + search_clock->level_increment_cs;
	}
	else if (mainboard.gubbins.turn == BLACK)
	{
		movesremaining = search_clock->black_remaining_moves;
		if (movesremaining == 0){movesremaining = 32;}
		mainsearch.endtime_cs = (search_clock->black_remaining_cs / movesremaining) + search_clock->level_increment_cs;
	}
	mainsearch.endtime_cs = mainsearch.endtime_cs - 5;
	if (mainsearch.endtime_cs < 5)
		mainsearch.endtime_cs = 5;
	search_start(&mainsearch, &mainboard);
	return;
}

int api_busy()
{
	return mainsearch.thinking;
}

void api_stop()
{
	search_stop(&mainsearch);
	return;
}

void api_update()
{
	int pvi, strst;
	mainsearch_output->depth = mainsearch.depthreached;
	mainsearch_output->depth_ext = mainsearch.extdepthreached;
	mainsearch_output->depth_qs = mainsearch.qsdepthreached;
	mainsearch_output->nodes = mainsearch.nodes;
	mainsearch_output->hashhits = mainsearch.hashhits;
	mainsearch_output->pv[0] = 0;
	strst = 1;
	for (pvi=0;pvi<mainsearch.pvl[0];++pvi)
	{
		strst+= board_sprintmove(&mainsearch_output->pv[strst - 1], &mainsearch.pv[0][pvi]);
	}
	mainsearch_output->score = mainsearch.score;
	mainsearch_output->time_cs = (clock() - mainsearch.starttime) / (CLOCKS_PER_SEC / 100);
	ui_updateoutput();
	return;
}

void api_bestmove()
{
	char move[8];
	char ponder[8];
	board_sprintmove(move, &mainsearch.bestmove);
	board_sprintmove(ponder, &mainsearch.pondermove);
	ui_bestmove(move, ponder);
	return;
}


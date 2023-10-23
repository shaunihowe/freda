// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#include "defs.h"

const int search_moveorder_pieces[] = {0, 0, 100, 300, 300, 500, 900, 10000};

void search_init(search_t *search)
{
	search->starttime = clock();
	search->endtime_cs = 5000;
	search->depthreached = 0;
	search->extdepthreached = 0;
	search->qsdepthreached = 0;
	search->score = 0;
	search->nodes = 0;
	search->hashhits = 0;
	search->bestmove = (move_t){0,0,0,0};
	search->pondermove = (move_t){0,0,0,0};
	search->depth = 0;
	memset(search->pv, 0, sizeof(search->pv));
	memset(search->history, 0, sizeof(search->history));
	return;
}

void search_start(search_t *search, board_t *board)
{
	int rc;
	search->board = *board;
	search->thinking = true;
	search->starttime = clock();
	search->depthreached = 0;
	search->extdepthreached = 0;
	search->qsdepthreached = 0;
	search->score = 0;
	search->nodes = 0;
	search->hashhits = 0;
	search->bestmove = (move_t){0,0,0,0};
	search->pondermove = (move_t){0,0,0,0};
	search->depth = 0;
	rc = pthread_create(&search->threadid, NULL, &thinkmove, (void *)search);
	if (rc)
	{
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		fflush(stdout);
	}
	return;
}

void search_stop(search_t *search)
{
	void *exitstatus;
	if (search->thinking == true)
	{
		search->thinking = false;
		// Wait for the search to terminate.
		pthread_join(search->threadid,&exitstatus);
	}
	return;
}

void *thinkmove(void *searchp)
{
	int time_cs, i;
	search_t *search = (search_t*)searchp;
	setjmp(search->checkpoint);
	search->onpv = false;
	for (search->depthreached=2;(search->depthreached < 64)&&(search->thinking == true);search->depthreached++)
	{
		search->score = search_alphabeta(search,-10001,10001,search->depthreached);
		search->bestmove = search->pv[0][0];
		search->pondermove = search->pv[0][1];
		if ((search->score < -9900) || (search->score > 9900))
			search->thinking = false;

		time_cs = (clock() - search->starttime) / (CLOCKS_PER_SEC / 100);
		if ((search->thinking == false) || (time_cs > 0))
			api_update();
		search->onpv = true;
	}
	api_bestmove();
	memset(search->pv, 0, sizeof(search->pv));
	memset(search->history, 0, sizeof(search->history));
	for (i = 0; i < hash_size; i++)
		if (hash_table[i].depth > 0)
			hash_table[i].depth--; //*/
	pthread_exit(NULL);
	return NULL;
}

int search_alphabeta(search_t *search, int alpha, int beta, int depth)
{
	move_t movelist[128];
	int movescore[128];
	int moves, spscore, imove, foundmove, pvi, legalmoves, ischeck, iscastle, reps, time_ms, useddepth, reduce;
	uint64_t hashkey;
	hash_t *hashentry;
	board_t *board;
	board = &search->board;

	if (search->depth > search->extdepthreached)
		search->extdepthreached = search->depth;

	if (search->depth)
	{
		reps = board_repeattest(board);
		if (reps >= 2)
			return 0;
		else if (reps == 1)
		{
			if (0 >= beta)
				return 0;
			if (0 > alpha)
				alpha = 0;
		}
	}

	ischeck = board_checktest(board, board->gubbins.turn);
	if (ischeck)
		++depth;

	hashkey = board->gubbins.hash & hash_mask;
	hashentry = &hash_table[hashkey];
	if (hashentry->hash == board->gubbins.hash)
	{
		if (hashentry->depth >= depth)
		{
			if (hashentry->betascore >= beta)
			{
				search->hashhits++;return beta;
			}
			if ((hashentry->alpha == 1)&&(hashentry->betascore <= alpha))
			{
				search->hashhits++;return alpha;
			}
		}
	}

	if (depth == 0)
		return search_qsearch(search, alpha, beta);

	search->nodes++;

	if (search->nodes % 1024 == 0)
	{
		time_ms = (clock() - search->starttime) / (CLOCKS_PER_SEC / 100);
		if (time_ms >= search->endtime_cs)
			search->thinking = false;
	}
	if (search->thinking == false)
		longjmp(search->checkpoint, 0);

	search->pvl[search->depth] = search->depth;

	if (search->depth >= MAXPLY - 1)
		return eval_full(board);

	legalmoves = 0;
	moves = board_generatecaptures(board, movelist);
	moves+= board_generatemoves(board, &movelist[moves]);
	foundmove = search_rankmoves_withhash(search, movelist, movescore, moves);
	if (foundmove == 2)
		return 10000;

	for (imove = 0; imove < moves; ++imove)
	{
		board_nextmove(movelist, movescore, moves, imove);
		iscastle = board_validcastle(board, &movelist[imove]);
		if (iscastle == CASTLE_ILLEGAL)
			continue;
		if ((iscastle > 0) && (ischeck))
			continue;
		board_domove(board, &movelist[imove]);search->depth++;
		// Test the move doesn't put our king under attack (illegal move!)
		if (board_checktest(board, 1 - board->gubbins.turn))
		{
			board_undomove(board);--search->depth;
			continue;
		}
		legalmoves++;

		if ((foundmove)&&(imove < 2))
			spscore = -search_alphabeta(search, -beta, -alpha, (useddepth = depth) - 1);
		else if (movelist[imove].capture + movelist[imove].promotion > 0)
			spscore = -search_alphabeta_alphafirst(search, -beta, -alpha, (useddepth = depth) - 1);
		else if ((!foundmove)&&(depth > 3)&&(-eval_full(board) <= alpha))
		{
			if (imove > 10)
				reduce = 1 + (depth / 3);
			else
				reduce = 1;
			spscore = -search_alphabeta_betatest(search, -alpha, (useddepth = (depth - reduce)) - 1);
			if (spscore > alpha)
				spscore = -search_alphabeta(search, -beta, -alpha, (useddepth = depth) - 1);
		}
		else
			spscore = -search_alphabeta(search, -beta, -alpha, (useddepth = depth) - 1);

		board_undomove(board);--search->depth;
		if (spscore > alpha)
		{
			search->history[movelist[imove].source][movelist[imove].destination] += useddepth;
			if (useddepth >= hashentry->depth)
			{
				hashentry->hash = board->gubbins.hash;
				hashentry->depth = useddepth;
				hashentry->bestmove = movelist[imove];
				hashentry->betascore = spscore;
				hashentry->alpha = 1;
				if (spscore >= beta)
					hashentry->alpha = 0;
			}
			if (spscore >= beta)
				return beta;
			alpha = spscore;
			search->pv[search->depth][search->depth] = movelist[imove];
			for (pvi = search->depth + 1; pvi < search->pvl[search->depth + 1]; ++pvi)
				search->pv[search->depth][pvi] = search->pv[search->depth + 1][pvi];
			search->pvl[search->depth] = search->pvl[search->depth + 1];
		}
	}

	if (legalmoves == 0)
	{
		if (ischeck)
			return -10000 + search->depth;
		return 0;
	}
	if (board->gubbins.fifty >= 100)
		return 0;
	return alpha;
}

int search_qsearch(search_t *search, int alpha, int beta)
{
	move_t movelist[128];
	int movescore[128];
	int moves, spscore, imove, pvi;
	uint64_t hashkey;
	hash_t *hashentry;
	board_t *board;
	board = &search->board;

	hashkey = board->gubbins.hash & hash_mask;
	hashentry = &hash_table[hashkey];
	if (hashentry->hash == board->gubbins.hash)
	{
		if (hashentry->betascore >= beta)
		{
			search->hashhits++;return beta;
		}
		if (hashentry->alpha == 1)
		{
			if (hashentry->betascore <= alpha)
			{
				search->hashhits++;return alpha;
			}
			else
			{
				search->hashhits++;return hashentry->betascore;
			}
		}
		if (hashentry->betascore > alpha)
			alpha = hashentry->betascore - 1;
	}

	if (search->depth > search->qsdepthreached)
		search->qsdepthreached = search->depth;

	search->nodes++;

	search->pvl[search->depth] = search->depth;

	if (search->depth >= MAXPLY - 1)
		return eval_full(board);

	spscore = eval_full(board);
	if (spscore >= beta)
		return beta;
	if (spscore > alpha)
		alpha = spscore;

	moves = board_generatecaptures(board, movelist);
	if (search_rankmoves(search, movelist, movescore, moves) == 2)
		return 10000;

	for (imove = 0; imove < moves; ++imove)
	{
		board_nextmove(movelist, movescore, moves, imove);
		board_domove(board, &movelist[imove]);search->depth++;
		spscore = -search_qsearch(search, -beta, -alpha);
		board_undomove(board);--search->depth;
		if (spscore > alpha)
		{
			if (spscore >= beta)
				return beta;
			alpha = spscore;
			search->pv[search->depth][search->depth] = movelist[imove];
			for (pvi = search->depth + 1; pvi < search->pvl[search->depth + 1]; ++pvi)
				search->pv[search->depth][pvi] = search->pv[search->depth + 1][pvi];
			search->pvl[search->depth] = search->pvl[search->depth + 1];
		}
	}
	return alpha;
}

int search_alphabeta_alphatest(search_t *search, int alpha, int depth)
{
	return search_alphabeta(search, alpha, alpha+1, depth);
}

int search_alphabeta_betatest(search_t *search, int beta, int depth)
{
	return search_alphabeta(search, beta-1, beta, depth);
}

int search_alphabeta_alphafirst(search_t *search, int alpha, int beta, int depth)
{
	if (search_alphabeta_alphatest(search, alpha, depth) <= alpha)
		return alpha;
	return search_alphabeta(search, alpha, beta, depth);
}

int search_alphabeta_betafirst(search_t *search, int alpha, int beta, int depth)
{
	if (search_alphabeta_betatest(search, beta, depth) >= beta)
		return beta;
	return search_alphabeta(search, alpha, beta, depth);
}

int search_rankmoves_withhash(search_t *search, move_t *movelist, int *movescore, int moves)
{
	int imove, wasonpv, foundmove = 0;
	uint64_t hashkey;
	hash_t *hashentry;

	hashkey = search->board.gubbins.hash & hash_mask;
	hashentry = &hash_table[hashkey];

	wasonpv = search->onpv;
	search->onpv = false;
	for (imove = 0; imove < moves; ++imove)
	{
		if (movelist[imove].capture == KING)
			return 2;
		if (wasonpv == true)
		{
			if (*(uint32_t*)&movelist[imove] == *(uint32_t*)&search->pv[0][search->depth])
			{
				movescore[imove] = 100000001;
				search->onpv = true;
				foundmove = 1;
				continue;
			}
		}
		if (hashentry->hash == search->board.gubbins.hash)
		{
			if (*(uint32_t*)&hashentry->bestmove == *(uint32_t*)&movelist[imove])
			{
				movescore[imove] = 100000000;
				foundmove = 1;
				continue;
			}
		}
		movescore[imove] = search_moveorder_pieces[movelist[imove].capture] + search_moveorder_pieces[movelist[imove].promotion];
		movescore[imove] += search->history[movelist[imove].source][movelist[imove].destination];
	}
	return foundmove;
}

int search_rankmoves(search_t *search, move_t *movelist, int *movescore, int moves)
{
	int imove, wasonpv, foundmove = 0;

	wasonpv = search->onpv;
	search->onpv = false;
	for (imove = 0; imove < moves; ++imove)
	{
		if (movelist[imove].capture == KING)
			return 2;
		if (wasonpv == true)
		{
			if (*(uint32_t*)&movelist[imove] == *(uint32_t*)&search->pv[0][search->depth])
			{
				movescore[imove] = 100000001;
				search->onpv = true;
				foundmove = 1;
				continue;
			}
		}
		movescore[imove] = search_moveorder_pieces[movelist[imove].capture] + search_moveorder_pieces[movelist[imove].promotion];
		movescore[imove] += search->history[movelist[imove].source][movelist[imove].destination];
	}
	return foundmove;
}

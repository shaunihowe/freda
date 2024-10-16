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
	search->scorebound = scorebound_exact;
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
	search->scorebound = scorebound_exact;
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
	int lower, upper, window, repeat;
	search_t *search = (search_t*)searchp;
	setjmp(search->checkpoint);
	search->onpv = false;
	for (search->depthreached=2;(search->depthreached < 64)&&(search->thinking == true);search->depthreached++)
	{
		window = 16;
		do
		{
			repeat = false;
			lower = search->score - window;
			upper = search->score + window;
			search->score = search_alphabeta(search,lower,upper,search->depthreached, 1);
			search->scorebound = scorebound_exact;
			if (search->score <= lower)
			{
				repeat = true;
				search->scorebound = scorebound_lower;
				window *= 2;
			}
			else if (search->score >= upper)
			{
				repeat = true;
				search->scorebound = scorebound_upper;
				window *= 2;
			}
		} while (repeat);
		if ((search->score < -9900) || (search->score > 9900))
			search->thinking = false;
		time_cs = (clock() - search->starttime) / (CLOCKS_PER_SEC / 100);
		if ((search->thinking == false) || (time_cs > 0))
			api_update();
		search->bestmove = search->pv[0][0];
		search->pondermove = search->pv[0][1];
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

int search_alphabeta(search_t *search, int alpha, int beta, int depth, int nullmove)
{
	move_t movelist[128];
	int movescore[128];
	int moves, spscore, imove, foundmove, pvi, legalmoves, ischeck, iscastle, reps, time_ms, useddepth, reduce;
	gubbins_t nullgubbins;
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
				search->hashhits++;
				return beta;
			}
			if ((hashentry->alpha == 1)&&(hashentry->betascore <= alpha))
			{
				search->hashhits++;
				return alpha;
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

	if (!search->onpv && nullmove && !ischeck && search->depth > 0 && depth > 3)
	{
		// todo: try null move (if not in check and not onpv)
		nullgubbins = board->gubbins;
		board->gubbins.hash ^= hash_ep[board->gubbins.ep];
		board->gubbins.ep = 0;
		board->gubbins.hash ^= hash_ep[board->gubbins.ep];
		board->gubbins.hash ^= hash_turn[board->gubbins.turn];
		board->gubbins.turn = 1 - board->gubbins.turn;
		board->gubbins.hash ^= hash_turn[board->gubbins.turn];
		spscore = -search_alphabeta_alphatest(search, -beta, (useddepth = (depth - 2)) - 1, 0);
		board->gubbins = nullgubbins;
		// if reduced search is >= beta then return beta
		if (spscore >= beta)
			return beta;
	}

	legalmoves = 0;
	moves = board_generatecaptures(board, movelist);
	moves += board_generatemoves(board, &movelist[moves]);
	foundmove = search_rankmoves_withhash(search, movelist, movescore, moves);
	if (foundmove == 2)
		return 10000;
	if ((!foundmove) && (!search->onpv) && (depth > 3))
	{
		foundmove = search_rankmoves_internal(search, movelist, movescore, moves);
		if (!ischeck)
			depth--;
	}

	for (imove = 0; imove < moves; ++imove)
	{
		board_nextmove(movelist, movescore, moves, imove);
		iscastle = board_validcastle(board, &movelist[imove]);
		if (iscastle == CASTLE_ILLEGAL)
			continue;
		if ((iscastle > 0) && (ischeck))
			continue;

		// calculate late move reductions
		reduce = 0;
		if ((foundmove) || (imove == 0) || (ischeck))
			// don't reduce on mainlines, 1st moves or if in check
			reduce = 0;
		else if ((depth > 3) && (movescore[imove] < alpha))
		{
			if (imove < 3)
				// lets only reduce by 1 on 2nd and 3rd moves
				reduce = 1;
			else
				reduce = 2;
		}

		board_domove(board, &movelist[imove]);search->depth++;
		// Test the move doesn't put our king under attack (illegal move!)
		if (board_checktest(board, 1 - board->gubbins.turn))
		{
			board_undomove(board);--search->depth;
			continue;
		}
		legalmoves++;

		if (reduce)
		{
			spscore = -search_alphabeta_betatest(search, -alpha, (useddepth = (depth - reduce)) - 1, 1);
			if (spscore > alpha)
				spscore = -search_alphabeta(search, -beta, -alpha, (useddepth = depth) - 1, 1);
		}
		else
			spscore = -search_alphabeta(search, -beta, -alpha, (useddepth = depth) - 1, 1);

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
			search->hashhits++;
			return beta;
		}
		if (hashentry->alpha == 1)
		{
			if (hashentry->betascore <= alpha)
			{
				search->hashhits++;
				return alpha;
			}
			else
			{
				search->hashhits++;
				return hashentry->betascore;
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

	// use standing pat and only search captures
	spscore = eval_full(board);
	if (spscore >= beta)
		return beta;
	if (spscore > alpha)
		alpha = spscore;
	moves = board_generatecaptures(board, movelist);

	if (search_rankmoves(search, movelist, movescore, moves) == 2)
		return 8000;

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

int search_alphabeta_alphatest(search_t *search, int alpha, int depth, int nullmove)
{
	return search_alphabeta(search, alpha, alpha+1, depth, nullmove);
}

int search_alphabeta_betatest(search_t *search, int beta, int depth, int nullmove)
{
	return search_alphabeta(search, beta-1, beta, depth, nullmove);
}

int search_alphabeta_alphafirst(search_t *search, int alpha, int beta, int depth, int nullmove)
{
	if (search_alphabeta_alphatest(search, alpha, depth, nullmove) <= alpha)
		return alpha;
	return search_alphabeta(search, alpha, beta, depth, nullmove);
}

int search_alphabeta_betafirst(search_t *search, int alpha, int beta, int depth, int nullmove)
{
	if (search_alphabeta_betatest(search, beta, depth, nullmove) >= beta)
		return beta;
	return search_alphabeta(search, alpha, beta, depth, nullmove);
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

int search_rankmoves_internal(search_t *search, move_t *movelist, int *movescore, int moves)
{
	int imove, spscore;
	board_t *board = &search->board;

	for (imove = 0; imove < moves; ++imove)
	{
		board_domove(board, &movelist[imove]);search->depth++;
		spscore = -search_qsearch(search, -10001, 10001);
		board_undomove(board);--search->depth;
		movescore[imove] = spscore;
	}
	return 0;
}


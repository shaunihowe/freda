// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#include "defs.h"

#include "eval_weights_default.h"

int numweights;
weights_t weights[] = {
	// variablename					elements    readaddr
    { "eval_pawn_value",			1,          &eval_pawn_value},
    { "eval_bishop_value",			2,          eval_bishop_value},
    { "eval_knight_value",			2,          eval_knight_value},
    { "eval_rook_value",			2,          eval_rook_value},
    { "eval_queen_value",			2,          eval_queen_value},
	{ "eval_pawn_sqr",   			64,			eval_pawn_sqr},
	{ "eval_pawn_passed",			64,			eval_pawn_passed},
	{ "eval_bishop_sqr",   			64,			eval_bishop_sqr},
	{ "eval_knight_sqr",   			64,			eval_knight_sqr},
	{ "eval_rook_sqr",   			64,			eval_rook_sqr},
	{ "eval_queen_sqr",   			64,			eval_queen_sqr},
	{ "eval_king_sqr_mg",  			64,			eval_king_sqr_mg},
	{ "eval_king_sqr_eg",  			64,			eval_king_sqr_eg},
	{ "eval_pawn_doubled_penalty",	1,          &eval_pawn_doubled_penalty},
	{ "eval_pawn_isolated_penalty",	1,          &eval_pawn_isolated_penalty},
	{ "eval_bishop_both",			1,          &eval_bishop_both},
	{ "eval_rook_semiopen",			1,          &eval_rook_semiopen},
	{ "eval_rook_open",				1,          &eval_rook_open},
	{ "eval_king_pawns",			4,          eval_king_pawns},
    { "eval_turn",					1,          &eval_turn},
    { NULL,							0,          NULL}
};

void weights_init()
{
	int iweight;
	numweights = 0;
	for (iweight = 0; weights[iweight].elements > 0; iweight++)
		numweights += weights[iweight].elements;
	weights_push(eval_weights_default);
	//weights_tune();
	//weights_export();
	return;
}

void weights_push(int *w)
{
	int sqr, iweight, curweight = 0;
	for (iweight = 0; weights[iweight].elements > 0; iweight++)
	{
		if (weights[iweight].elements == 1)
		{
			*weights[iweight].readaddr = w[curweight];
			curweight++;
		}
		else if (weights[iweight].elements > 1)
		{
			for (sqr = 0; sqr < weights[iweight].elements; sqr++)
			{
				weights[iweight].readaddr[sqr] = w[curweight];
				curweight++;
			}
		}
	}
	// anchor pawn value to 1000
	eval_pawn_value = 1000;
	return;
}

void weights_pull(int *w)
{
	int sqr, iweight, curweight = 0;
	for (iweight = 0; weights[iweight].elements > 0; iweight++)
	{
		if (weights[iweight].elements == 1)
		{
			w[curweight] = *weights[iweight].readaddr;
			curweight++;
		}
		else if (weights[iweight].elements > 1)
		{
			for (sqr = 0; sqr < weights[iweight].elements; sqr++)
			{
				w[curweight] = weights[iweight].readaddr[sqr];
				curweight++;
			}
		}
	}
	return;
}

void weights_export()
{
	int file, sqr, piece, iweight;
	FILE* out;
	out = fopen("eval_weights_default.h","w");

	fprintf(out, "#ifndef __INC_EVAL_WEIGHTS_DEFAULT__\n#define __INC_EVAL_WEIGHTS_DEFAULT__\n\n");

	fprintf(out, "// %i Weights\n\n", numweights);

	fprintf(out, "int eval_weights_default[] = {\n");

	for (iweight = 0; weights[iweight].elements > 0; iweight++)
	{
		fprintf(out, "\t// %s\n", weights[iweight].variablename);
		if (weights[iweight].elements == 1)
		{
			fprintf(out, "\t%4i,\n", *weights[iweight].readaddr);
		}
		else if (weights[iweight].elements == 64)
		{
			file = 0;
			for (sqr = 0; sqr < 64; sqr++)
			{
				if (file == 0)
					fprintf(out, "\t");
				fprintf(out, "%4i,", weights[iweight].readaddr[sqr]);
				if (file == 7)
					fprintf(out, "\n");
				file++;if (file > 7){file = 0;}
			}
		}
		else
		{
			for (piece = 0; piece < weights[iweight].elements; piece++)
			{
				fprintf(out, "\t%4i,", weights[iweight].readaddr[piece]);
			}
			fprintf(out, "\n");
		}
	}

	fprintf(out, "\t// end of eval_weights_default[]\n\t%4i\n};\n\n#endif\n",0);

	fclose(out);
	return;
}

void weights_tune()
{
	int best_genome[2000];
	int best_genome_score;
	int candidate_genome[2000];
	int candidate_genome_score;
	float perf_genome[2000];
	int perf_count[2000];
	int rounds, generations, games, iweight, perf_progress;
	FILE *data;
	data = fopen("data.txt","r");
	weights_pull(best_genome);
	for (iweight = 0; iweight < numweights; iweight++)
	{
		perf_genome[iweight] = (float)best_genome[iweight];
		perf_count[iweight] = 1;
	}
	generations = 0;
	games =  0;
	perf_progress = 0;
	printf("Generation: %4i\n", generations);
	for (rounds = 0; !feof(data); rounds++)
	{
		printf("Finding Candidate");fflush(stdout);
		weights_tune_findcandidate(data, best_genome, candidate_genome);
		printf(", Playing Match");fflush(stdout);
		games += weights_tune_match(data, best_genome, candidate_genome, &best_genome_score, &candidate_genome_score);
		if (candidate_genome_score > best_genome_score)
		{
			perf_progress = 0;
			for (iweight = 0; iweight < numweights; iweight++)
			{
				if (perf_count[iweight] > 64)
					perf_progress += 64;
				else
					perf_progress += perf_count[iweight];
				if (candidate_genome[iweight] != best_genome[iweight])
				{
					perf_genome[iweight] *= (float)perf_count[iweight];
					if (candidate_genome[iweight] > best_genome[iweight])
						perf_genome[iweight] += (float)(best_genome[iweight] + 1000);
					else if (candidate_genome[iweight] < best_genome[iweight])
						perf_genome[iweight] += (float)(best_genome[iweight] - 1000);
					perf_count[iweight]++;
					perf_genome[iweight] /= (float)perf_count[iweight];
					if (perf_count[iweight] > 32)
						candidate_genome[iweight] = (int)perf_genome[iweight];
				}
			}
			generations++;
			weights_push(candidate_genome);
			weights_export();
			printf("Generation: %4i, Games Played: %i, Progress: %i/%i\n", generations, games, perf_progress, numweights * 64);fflush(stdout);
			weights_pull(best_genome);
		}
	}
	fclose(data);
	printf("Tuning Finished.\n");
	fflush(stdout);
	return;
}

int weights_simulate(board_t *board, int alpha, int beta, int depth)
{
	move_t movelist[128];
	int moves, spscore, imove, legalmoves, ischeck;

	ischeck = board_checktest(board, board->gubbins.turn);
	if (ischeck)
		depth++;

	if (depth <= 0)
		return board_qsearch(board, alpha, beta);

	legalmoves = 0;
	moves = board_generatecaptures(board, movelist);
	moves += board_generatemoves(board, &movelist[moves]);
	for (imove = 0; imove < moves; ++imove)
	{
		if (movelist[imove].capture == KING)
			return beta;
		board_domove(board, &movelist[imove]);
		if (board_checktest(board, 1 - board->gubbins.turn))
		{
			board_undomove(board);
			continue;
		}
		legalmoves++;
		spscore = -weights_simulate(board, -beta, -alpha, depth - 1);
		board_undomove(board);
		if (spscore >= beta)
			return beta;
		if (spscore > alpha)
			alpha = spscore;
	}

	if (legalmoves == 0)
	{
		if (ischeck)
			return beta;
		return 0;
	}
	return alpha;
}

void weights_tune_findcandidate(FILE *data, int *base_genome, int *candidate_genome)
{
	int iweight, rounds;
	char input[2048];
	char fenstring[256];
    char turn[2],castle[8],ep[2];
	int target_score;
	board_t board;
	int candidate_genome_score;
	int test_genome[2000];
	int test_genome_score;
	int delta_genome[2000];
	weights_push(base_genome);
	weights_pull(candidate_genome);
	weights_pull(test_genome);
	for (iweight = 0; iweight < numweights; iweight++)
		delta_genome[iweight] = 0;
	for (rounds = 0; (!feof(data)) && (rounds < 128); rounds++)
	{
		if (rounds % 32 == 31)
		{
			printf(".");
			fflush(stdout);
		}
		if (!fgets(input, 2048, data)){break;}
		if (input[0] == '\n'){break;}
		sscanf(input, "%i %s %s %s %s", &target_score, fenstring, turn, castle, ep);
		strcat(fenstring," ");
		strcat(fenstring,turn);
		strcat(fenstring," ");
		strcat(fenstring,castle);
		strcat(fenstring," ");
		strcat(fenstring,ep);
		board_setboard(&board, fenstring);
		if (board.gubbins.turn == BLACK)
			target_score = -target_score;
		weights_push(candidate_genome);
		candidate_genome_score = weights_simulate(&board, -10001, 10001, 2) - target_score;
		if (candidate_genome_score < 0){candidate_genome_score = -candidate_genome_score;}
		for (iweight = 0; iweight < numweights; iweight++)
		{
			if (rand() % 2 == 0)
				test_genome[iweight] = candidate_genome[iweight] + 10;
			else
				test_genome[iweight] = candidate_genome[iweight] - 10;
			weights_push(test_genome);
			test_genome_score = weights_simulate(&board, -10001, 10001, 2) - target_score;
			if (test_genome_score < 0){test_genome_score = -test_genome_score;}
			if (test_genome_score < candidate_genome_score)
			{
				if (test_genome[iweight] > candidate_genome[iweight])
					delta_genome[iweight]++;
				else if (test_genome[iweight] < candidate_genome[iweight])
					delta_genome[iweight]--;
			}
			test_genome[iweight] = candidate_genome[iweight];
		}
	}
	for (iweight = 0; iweight < numweights; iweight++)
	{
		if (delta_genome[iweight] > 30)
			delta_genome[iweight] = 30;
		else if (delta_genome[iweight] < -30)
			delta_genome[iweight] = -30;
		candidate_genome[iweight] += delta_genome[iweight];
	}
	return;
}

int weights_tune_match(FILE *data, int *best_genome, int *candidate_genome, int *best_genome_score, int *candidate_genome_score)
{
	int games;
	char input[2048];
	char fenstring[256];
    char turn[2],castle[8],ep[2];
	int target_score;
	board_t board;
	int delta_score;
	int b_g_score, c_g_score;
	float inc;
	*best_genome_score = 0;
	*candidate_genome_score = 0;
	games = 1;
	delta_score = 0;
	for (games = games; (delta_score > -50) && (delta_score < 50) && (!feof(data)); games++)
	{
		if (!fgets(input, 2048, data)){break;}
		if (input[0] == '\n'){break;}
		sscanf(input, "%i %s %s %s %s", &target_score, fenstring, turn, castle, ep);
		strcat(fenstring," ");
		strcat(fenstring,turn);
		strcat(fenstring," ");
		strcat(fenstring,castle);
		strcat(fenstring," ");
		strcat(fenstring,ep);
		board_setboard(&board, fenstring);
		if (board.gubbins.turn == BLACK)
			target_score = -target_score;
		weights_push(best_genome);
		b_g_score = weights_simulate(&board, -10001, 10001, 2) - target_score;
		if (b_g_score < 0){b_g_score = -b_g_score;}
		weights_push(candidate_genome);
		c_g_score = weights_simulate(&board, -10001, 10001, 2) - target_score;
		if (c_g_score < 0){c_g_score = -c_g_score;}
		if (b_g_score < c_g_score)
			*best_genome_score = *best_genome_score + 1;
		else if (c_g_score < b_g_score)
			*candidate_genome_score = *candidate_genome_score + 1;
		delta_score = *candidate_genome_score - *best_genome_score;
	}
	inc = (float)*candidate_genome_score + (float)*best_genome_score;
	inc = (float)*candidate_genome_score / inc;
	printf(", Result: %i - %i (%2.1f%%) %i games\n", *best_genome_score, *candidate_genome_score, inc * 100.0f, games);
	fflush(stdout);
	return games;
}

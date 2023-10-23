// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#include "defs.h"

#include "eval_weights_default.h"

int numweights;
weights_t weights[] = {
	// variablename					elements    readaddr
    { "eval_piece_value",			8,          eval_piece_value},
    { "eval_pawn_sqr",   			64,			eval_pawn_sqr},
	{ "eval_pawn_passed_sqr",   	64,			eval_pawn_passed_sqr},
	{ "eval_bishop_sqr",   			64,			eval_bishop_sqr},
	{ "eval_knight_sqr",   			64,			eval_knight_sqr},
	{ "eval_rook_sqr",   			64,			eval_rook_sqr},
	{ "eval_queen_sqr",   			64,			eval_queen_sqr},
	{ "eval_king_sqr_mg",  			64,			eval_king_sqr_mg},
	{ "eval_king_sqr_eg",  			64,			eval_king_sqr_eg},
	{ "eval_pawn_doubled_penalty",	1,          &eval_pawn_doubled_penalty},
	{ "eval_pawn_isolated_penalty",	1,          &eval_pawn_isolated_penalty},
	{ "eval_pawn_backward_penalty",	1,          &eval_pawn_backward_penalty},
	{ "eval_rook_semiopen",			1,          &eval_rook_semiopen},
	{ "eval_rook_open",				1,          &eval_rook_open},
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
	int base_genome[2000];
	int evol_genome[2000];
	int curtest, iweight, evol_count, epochs, rounds;
	int best_genome[2000];
	int test_genome[2000];
	int best_genome_score;
	int test_genome_score;
	evol_count = 1;
	weights_pull(evol_genome);
	for (epochs = 0; epochs < 64; epochs++)
	{
		weights_pull(base_genome);
		for (rounds = 0; rounds < 1024; rounds++)
		{
			best_genome_score = 10000;
			for (curtest = 0; curtest < 64; curtest++)
			{
				for (iweight = 0; iweight < numweights; iweight++)
				{
					if (rand() % 2 == 0)
						test_genome[iweight] = base_genome[iweight] + 10;
					else
						test_genome[iweight] = base_genome[iweight] - 10;
				}
				weights_push(test_genome);
				//simlulate
				test_genome_score = rand() % 1024;
				if (test_genome_score < best_genome_score)
				{
					weights_pull(best_genome);
					best_genome_score = test_genome_score;
				}
			}
			evol_count++;
			for (iweight = 0; iweight < numweights; iweight++)
			{
				if (best_genome[iweight] > base_genome[iweight])
					evol_genome[iweight] += base_genome[iweight] + 1000;
				else if (best_genome[iweight] < base_genome[iweight])
					evol_genome[iweight] += base_genome[iweight] - 1000;
				else
					evol_genome[iweight] += base_genome[iweight];
			}
		}
		for (iweight = 0; iweight < numweights; iweight++)
		{
			base_genome[iweight] = evol_genome[iweight] / evol_count;
		}
		weights_push(base_genome);
		weights_export();
	}
	weights_export();
	return;
}

// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#include "defs.h"

#include "eval_weights_mg.h"
#include "eval_weights_eg.h"

int numweights;
weights_t weights[] = {
	// variablename					elements    readaddr
    { "eval_piece_value",			8,          eval_piece_value},
	{ "eval_pawn_sqr",   			64,			eval_pawn_sqr},
	{ "eval_pawn_passed",			64,			eval_pawn_passed},
	{ "eval_bishop_sqr",   			64,			eval_bishop_sqr},
	{ "eval_knight_sqr",   			64,			eval_knight_sqr},
	{ "eval_rook_sqr",   			64,			eval_rook_sqr},
	{ "eval_queen_sqr",   			64,			eval_queen_sqr},
	{ "eval_king_sqr",  			64,			eval_king_sqr},
	{ "eval_pawn_doubled_penalty",	1,          &eval_pawn_doubled_penalty},
	{ "eval_pawn_isolated_penalty",	1,          &eval_pawn_isolated_penalty},
	{ "eval_bishop_both",			1,          &eval_bishop_both},
	{ "eval_rook_semiopen",			1,          &eval_rook_semiopen},
	{ "eval_rook_open",				1,          &eval_rook_open},
	{ "eval_king_pawns",			4,          eval_king_pawns},
	{ "eval_nomatemat",				1,			&eval_nomatemat},
    { "eval_turn",					1,          &eval_turn},
    { NULL,							0,          NULL}
};

void weights_init()
{
	int iweight;
	numweights = 0;
	for (iweight = 0; weights[iweight].elements > 0; iweight++)
		numweights += weights[iweight].elements;
	weights_push(eval_weights_mg, eval_weights_eg);
	//weights_export();
	return;
}

void weights_push(int32_t *w_mg, int32_t *w_eg)
{
	int sqr, iweight, curweight = 0;
	for (iweight = 0; weights[iweight].elements > 0; iweight++)
	{
		if (weights[iweight].elements == 1)
		{
			*weights[iweight].readaddr = (w_eg[curweight] << 16) + w_mg[curweight];
			curweight++;
		}
		else if (weights[iweight].elements > 1)
		{
			for (sqr = 0; sqr < weights[iweight].elements; sqr++)
			{
				weights[iweight].readaddr[sqr] = (w_eg[curweight] << 16) + w_mg[curweight];
				curweight++;
			}
		}
	}
	return;
}

void weights_pull(int32_t *w_mg, int32_t *w_eg)
{
	// int16_t(score)
	// score + 0x8000 >> 16
	int sqr, iweight, curweight = 0;
	for (iweight = 0; weights[iweight].elements > 0; iweight++)
	{
		if (weights[iweight].elements == 1)
		{
			w_mg[curweight] = (int16_t)*weights[iweight].readaddr;
			w_eg[curweight] = (*weights[iweight].readaddr + 0x8000) >> 16;
			curweight++;
		}
		else if (weights[iweight].elements > 1)
		{
			for (sqr = 0; sqr < weights[iweight].elements; sqr++)
			{
				w_mg[curweight] = (int16_t)weights[iweight].readaddr[sqr];
				w_eg[curweight] = (weights[iweight].readaddr[sqr] + 0x8000) >> 16;
				curweight++;
			}
		}
	}
	return;
}

void weights_export()
{
	// int16_t(score)
	// score + 0x8000 >> 16
	int file, sqr, piece, iweight;
	FILE* out_mg;
	FILE* out_eg;
	out_mg = fopen("eval_weights_mg.h","w");
	fprintf(out_mg, "#ifndef __INC_EVAL_WEIGHTS_MG__\n#define __INC_EVAL_WEIGHTS_MG__\n\n");
	fprintf(out_mg, "// %i Weights\n\n", numweights);
	fprintf(out_mg, "int32_t eval_weights_mg[] = {\n");
	out_eg = fopen("eval_weights_eg.h","w");
	fprintf(out_eg, "#ifndef __INC_EVAL_WEIGHTS_EG__\n#define __INC_EVAL_WEIGHTS_EG__\n\n");
	fprintf(out_eg, "// %i Weights\n\n", numweights);
	fprintf(out_eg, "int32_t eval_weights_eg[] = {\n");

	for (iweight = 0; weights[iweight].elements > 0; iweight++)
	{
		fprintf(out_mg, "\t// %s\n", weights[iweight].variablename);
		fprintf(out_eg, "\t// %s\n", weights[iweight].variablename);
		if (weights[iweight].elements == 1)
		{
			fprintf(out_mg, "\t%4i,\n", (int16_t)*weights[iweight].readaddr);
			fprintf(out_eg, "\t%4i,\n", (*weights[iweight].readaddr + 0x8000) >> 16);
		}
		else if (weights[iweight].elements == 64)
		{
			file = 0;
			for (sqr = 0; sqr < 64; sqr++)
			{
				if (file == 0)
				{
					fprintf(out_mg, "\t");
					fprintf(out_eg, "\t");
				}
				fprintf(out_mg, "%4i,", (int16_t)weights[iweight].readaddr[sqr]);
				fprintf(out_eg, "%4i,", (weights[iweight].readaddr[sqr] + 0x8000) >> 16);
				if (file == 7)
				{
					fprintf(out_mg, "\n");
					fprintf(out_eg, "\n");
				}
				file++;if (file > 7){file = 0;}
			}
		}
		else
		{
			for (piece = 0; piece < weights[iweight].elements; piece++)
			{
				fprintf(out_mg, "\t%4i,", (int16_t)weights[iweight].readaddr[piece]);
				fprintf(out_eg, "\t%4i,", (weights[iweight].readaddr[piece] + 0x8000) >> 16);
			}
			fprintf(out_mg, "\n");
			fprintf(out_eg, "\n");
		}
	}

	fprintf(out_mg, "\t// end of eval_weights_mg[]\n\t%4i\n};\n\n#endif\n",0);
	fclose(out_mg);
	fprintf(out_eg, "\t// end of eval_weights_eg[]\n\t%4i\n};\n\n#endif\n",0);
	fclose(out_eg);
	return;
}

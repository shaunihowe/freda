// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#include "api.h"

#include <stdio.h>
#include <string.h>

typedef enum
{
	m_mainloop,
	m_position,
		m_position_fen,
		m_position_moves,
	m_go
} uci_mode;

search_output_t search_output;

void uci_command()
{
	printf("id name %s %i.%i\n", VERSION_NAME, VERSION_MAJOR, VERSION_MINOR);
	printf("id author %s\n", VERSION_AUTHOR);
	//printf("option name UCI_AnalyseMode type check default true\n");
	//printf("option name Hash type spin default 4 min 1 max 128\n");
	printf("uciok\n");
	fflush(stdout);
	return;
}

void ui_updateoutput()
{
	if (search_output.time_ms == 0)
		search_output.time_ms = 1;
	printf("info depth %i seldepth %i ", search_output.depth, search_output.depth_qs);
	printf("time %i nodes %i nps %i ", search_output.time_ms * 10, search_output.nodes, (search_output.nodes / search_output.time_ms) * 100);
	printf("score cp %i ", search_output.score);
	printf("pv %s\n", search_output.pv);
	fflush(stdout);
	return;
}

void ui_bestmove(const char *move, const char *ponder)
{
	printf("bestmove %s ponder %s\n", move, ponder);
	fflush(stdout);
	return;
}

int main()
{
	char fenstring[256];
	char input[2048], *command;
	char delim[10] = " \t\n\r";
	fenstring[0] = 0;
	api_init();
	int programclose = false;
	do
	{
		fflush(stdout);
		if (!fgets(input, 2048, stdin)){return 0;}
		if (input[0] == '\n'){continue;}
		command = strtok(input, delim);
		uci_mode mode = m_mainloop;
		do
		{
			if (mode == m_mainloop)
			{
				if (!strcmp(command, "uci"))
					uci_command();
				else if (!strcmp(command, "isready"))
					printf("readyok\n");
				else if (!strcmp(command, "ucinewgame"))
					api_init();
				else if (!strcmp(command, "show"))
					api_showposition();
				else if (!strcmp(command, "position"))
					mode = m_position;
				else if (!strcmp(command, "go"))
					api_go(&search_output);
				else if (!strcmp(command, "moves"))
					mode = m_position_moves;
				else if (!strcmp(command, "stop"))
					api_stop();
				else if (!strcmp(command, "quit"))
					programclose = true;
			}
			else if (mode == m_position)
			{
				if (!strcmp(command, "fen"))
				{
					mode = m_position_fen;
					fenstring[0] = 0;
				}
				else if (!strcmp(command, "startpos"))
					api_setposition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
				else if (!strcmp(command, "moves"))
					mode = m_position_moves;
			}
			else if (mode == m_position_fen)
			{
				do
				{
					if (!strcmp(command, "moves"))
					{
						mode = m_position_moves;
						break;
					}
					else
					{
					    strcat(fenstring,command);
					    strcat(fenstring," ");
					}
					command = strtok(NULL, delim);
				} while (command != NULL);
				api_setposition(fenstring);
			}
			else if (mode == m_position_moves)
			{
        		if (!(command[0] < 'a' || command[0] > 'h' || command[1] < '0' || command[1] > '8' || command[2] < 'a' || command[2] > 'h' || command[3] < '0' || command[3] > '8'))
				api_move(command);
			}
			command = strtok(NULL, delim);
		} while (command != NULL);
	} while (!programclose);
	api_stop();
	return 0;
}


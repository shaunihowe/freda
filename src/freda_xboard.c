// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#include "api.h"

#include <stdio.h>
#include <string.h>

typedef enum
{
	m_force,
	m_go_white,
	m_go_black,
	m_analyze
} xboard_mode;
xboard_mode mode;

search_output_t search_output;
clock_info_t search_clock = {0,6000,0,0,0,6000,6000};

void modecheck()
{
	if (api_busy() == false)
	{
		if (mode == m_analyze)
		{
			search_clock = (clock_info_t){0,99999900,0,0,0,99999900,99999900};
			api_go(&search_output, &search_clock);
		}
		else if ((mode == m_go_white)&&(api_getstatus() == status_turn_white))
			api_go(&search_output, &search_clock);
		else if ((mode == m_go_black)&&(api_getstatus() == status_turn_black))
			api_go(&search_output, &search_clock);
	}
	return;
}

void xboard_command()
{
    printf("feature done=0\n");             // tell xboard to wait for all feature tags
    fflush(stdout);
    printf("feature myname=\"%s %i.%i\"\n", VERSION_NAME, VERSION_MAJOR, VERSION_MINOR);
    printf("feature playother=1\n");
    printf("feature usermove=0\n");
    printf("feature time=1\n");
    printf("feature draw=0\n");
    printf("feature colors=1\n");
    printf("feature setboard=0\n");
    printf("feature analyze=1\n");
    printf("feature sigint=0\n");
    printf("feature sigterm=0\n");
    printf("feature reuse=1\n");
    printf("feature done=1\n");             // tell xboard all feature tags are done
    fflush(stdout);
    return;
}

void ui_updateoutput()
{
	printf("%i %i %i %i ", search_output.depth, search_output.score, search_output.time_cs, search_output.nodes);
	printf("%s\n", search_output.pv);
	fflush(stdout);
	return;
}

void ui_bestmove(const char *move, const char *ponder)
{
	if ((mode == m_go_white)&&(api_getstatus() == status_turn_white))
	{
		printf("move %s\n", move);
		api_move(move);
	}
	else if ((mode == m_go_black)&&(api_getstatus() == status_turn_black))
	{
		printf("move %s\n", move);
		api_move(move);
	}
	fflush(stdout);
	return;
}

int main()
{
	char input[2048], command[256];
	char fenstring[256];
    char turn[2],castle[8],ep[2];
	int m,t,i;
	api_init();
	mode = m_go_black;
	int programclose = false;
	do
	{
		fflush(stdout);
		if (!fgets(input, 2048, stdin)){return 0;}
		if (input[0] == '\n'){continue;}
		sscanf(input, "%s", command);
		if (!strcmp(command, "xboard"))
			xboard_command();
		else if (!strcmp(command, "new"))
		{
			api_init();
			mode = m_go_black;
			search_clock = (clock_info_t){0,6000,0,0,0,6000,6000};
		}
		else if (!strcmp(command, "force"))
		{
			mode = m_force;
			api_stop();
		}
		else if (!strcmp(command, "white"))
		{
			mode = m_force;
			api_stop();
			mode = m_go_white;
		}
		else if (!strcmp(command, "black"))
		{
			mode = m_force;
			api_stop();
			mode = m_go_black;
		}
		else if (!strcmp(command, "level"))
		{
			sscanf(input, "level %i %i %i", &m,&t,&i);
			search_clock.level_moves = m;
			search_clock.level_time_cs = t;
			search_clock.level_increment_cs = i;
			search_clock.white_remaining_moves = m;
			search_clock.black_remaining_moves = m;
			search_clock.white_remaining_cs = t;
			search_clock.black_remaining_cs = t;
		}
		else if (!strcmp(command, "time"))
		{
			sscanf(input, "time %i", &t);
			if (mode == m_go_white)
				search_clock.white_remaining_cs = t;
			else if (mode == m_go_black)
				search_clock.black_remaining_cs = t;
		}
		else if (!strcmp(command, "otim"))
		{
			sscanf(input, "time %i", &t);
			if (mode == m_go_white)
				search_clock.black_remaining_cs = t;
			else if (mode == m_go_black)
				search_clock.white_remaining_cs = t;
		}
		else if (!strcmp(command, "go"))
		{
			mode = m_force;
			api_stop();
			if (api_getstatus() == status_turn_white)
				mode = m_go_white;
			else if (api_getstatus() == status_turn_black)
				mode = m_go_black;
		}
		else if (!strcmp(command, "playother"))
		{
			mode = m_force;
			api_stop();
			if (api_getstatus() == status_turn_white)
				mode = m_go_black;
			else if (api_getstatus() == status_turn_black)
				mode = m_go_white;
		}
		else if (!strcmp(command, "setboard"))
		{
			mode = m_force;
			api_init();
			sscanf(input, "setboard %s %s %s %s", fenstring,turn,castle,ep);
			strcat(fenstring," ");
			strcat(fenstring,turn);
			strcat(fenstring," ");
			strcat(fenstring,castle);
			strcat(fenstring," ");
			strcat(fenstring,ep);
			api_setposition(fenstring);
		}
		else if (!strcmp(command, "show"))
		{
			api_showposition();
		}
		else if (!strcmp(command, "analyze"))
		{
			mode = m_analyze;
			api_stop();
		}
		else if (!strcmp(command, "undo"))
			api_undomove();
		else if (!strcmp(command, "remove"))
		{
			api_undomove();
			api_undomove();
		}
		else if (!strcmp(command, "exit"))
		{
			mode = m_force;
			api_stop();
		}
		else if (!(command[0] < 'a' || command[0] > 'h' || command[1] < '0' || command[1] > '8' || command[2] < 'a' || command[2] > 'h' || command[3] < '0' || command[3] > '8'))
			api_move(command);
		else if (!strcmp(command, "quit"))
			programclose = true;
		modecheck();
	} while (!programclose);
	api_stop();
	return 0;
}

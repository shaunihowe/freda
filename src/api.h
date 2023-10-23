// Freda Chess Engine - By Shaun Howe
// https://github.com/shaunihowe/freda
// BSD 2-Clause License - see file 'LICENSE' for more information

#ifndef __INC_API__ 
#define __INC_API__ 

#include <stdint.h>
#include <math.h>

#define VERSION_NAME	"Freda"
#define VERSION_AUTHOR	"Shaun Howe"
#define VERSION_MAJOR	0
#define VERSION_MINOR	99

#define true 1
#define false 0

typedef struct {
	int level_moves;
	int level_time_cs;
	int level_increment_cs;
	int white_remaining_moves;
	int black_remaining_moves;
	int white_remaining_cs;
	int black_remaining_cs;
} clock_info_t;
//const clock_info_t clock_template_allin1 = {-1,6000,0,-1,-1,6000,6000};

typedef enum {
	status_turn_white,
	status_turn_black,
	status_result_white_checkmate,
	status_result_black_checkmate,
	status_result_draw_stalemate,
	status_result_draw_repetition,
	status_result_draw_nomatematerial,
	status_result_draw_50move
} game_status_t;

typedef struct {
	int depth;
	int depth_ext;
	int depth_qs;
	int score;
	int time_cs;
	int nodes;
	int hashhits;
	char pv[512];
} search_output_t;

void api_init();
void api_setposition(const char *fenstring);
void api_getposition(char *fenstring);
void api_showposition();
game_status_t api_getstatus();
void api_move(const char *move);
void api_undomove();
void api_go(search_output_t *search_output, clock_info_t *search_clock);
int api_busy();
void api_stop();
void api_update();
void api_bestmove();

void ui_updateoutput();
void ui_bestmove(const char *move, const char *ponder);

#endif

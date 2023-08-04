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
#define VERSION_MINOR	9

#define true 1
#define false 0

typedef struct {
	int white_time_ms;
	int black_time_ms;
	int white_increment_ms;
	int black_increment_ms;
	int white_moves_remaining;
	int black_moves_remaining;
} clock_info_t;
//const clock_info_t clock_template_allin1 = {6000,6000,0,0,-1,-1};
//const clock_info_t clock_template_ccrltwoone = {12000,12000,100,100,-1,-1};
//const clock_info_t clock_template_ccrl40in15 = {90000,90000,0,0,40,40};
//const clock_info_t clock_template_fischerfivethree = {30000,30000,300,300,-1,-1};
//const clock_info_t clock_template_fischertenfive = {60000,60000,500,500,-1,-1};

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
	int time_ms;
	int nodes;
	char pv[512];
} search_output_t;

void api_init();
void api_setposition(const char *fenstring);
void api_getposition(char *fenstring);
void api_showposition();
game_status_t api_getstatus();
void api_move(const char *move);
void api_undomove();
void api_go(search_output_t *search_output);
int api_busy();
void api_stop();
void api_update();
void api_bestmove();

void ui_updateoutput();
void ui_bestmove(const char *move, const char *ponder);

#endif

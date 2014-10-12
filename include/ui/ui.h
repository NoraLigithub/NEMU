#ifndef __UI_H__
#define __UI_H__

#include "common.h"

enum { END, STOP, RUNNING, INT};
extern int nemu_state;
int cmd_p(char* p);
uint32_t expr(char *e,bool *success);
#endif

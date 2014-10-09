#ifndef __UI_BREAKPOINT_H__
#define __UI_BREAKPOINT_H__

#include "common.h"

#define INT3_CODE 0xcc
#define NR_BP 32

typedef struct breakpoint {
	int NO;
	unsigned address;
	unsigned short material;
	struct breakpoint *next;

	/* TODO: Add more members if necessary */


} BP;

void set_bp(char *p);
BP* new_bp();
void print_bp();
void free_bp(int number);
BP* ret_head();
#endif

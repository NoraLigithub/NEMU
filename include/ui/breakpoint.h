#ifndef __UI_BREAKPOINT_H__
#define __UI_BREAKPOINT_H__

#include "common.h"

#define INT3_CODE 0xcc

typedef struct breakpoint {
	int NO;
	unsigned address;
	unsigned short material;
	bool enb;
	bool used;
	struct breakpoint *next;

	/* TODO: Add more members if necessary */


} BP;

void set_bp(char *p);
BP* new_bp();
#endif

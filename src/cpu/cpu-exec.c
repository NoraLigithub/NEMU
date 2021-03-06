#include "ui/ui.h"

#include "nemu.h"

#include <setjmp.h>

#include "ui/breakpoint.h"

#define LOADER_START 0x100000

extern bool is_Break;

int exec(swaddr_t);
void load_prog();
void init_dram();

char assembly[40];
jmp_buf jbuf;	/* Make it easy to perform exception handling */

extern uint8_t loader [];
extern uint32_t loader_len;

extern int quiet;

void restart() {
	/* Perform some initialization to restart a program */
	load_prog();
	memcpy(hwa_to_va(LOADER_START), loader, loader_len);

	cpu.esp=0x8000000;
	cpu.ebp=0x0;
	cpu.eip = LOADER_START;
	cpu.EFLAGS._32=0X2;
	init_dram();
}

static void print_bin_instr(swaddr_t eip, int len) {
	int i;
	printf("%8x:   ", eip);
	for(i = 0; i < len; i ++) {
		printf("%02x ", swaddr_read(eip + i, 1));
	}
	printf("%*.s", 50 - (12 + 3 * len), "");
}

void cpu_exec(volatile uint32_t n) {
	volatile uint32_t n_temp = n;

	setjmp(jbuf);
	for(; n > 0; n --) {
		swaddr_t eip_temp = cpu.eip;
		BP* current;
		int instr_len;
		if(is_Break == true && swaddr_read(cpu.eip-1,1) == INT3_CODE){
			cpu.eip--;
			current=ret_head();
			while(current->address != cpu.eip)
			   	current = current->next;
		  	swaddr_write(cpu.eip,1,current->material);
		}
	    instr_len = exec(cpu.eip);
		if(is_Break == true && swaddr_read(cpu.eip,1) != INT3_CODE){
		        	swaddr_write(cpu.eip,1,INT3_CODE);
					is_Break=false;
			}
     	cpu.eip += instr_len;
		if(n_temp != -1 || (enable_debug && !quiet)) {
			print_bin_instr(eip_temp, instr_len);
			puts(assembly);
		}

		if(nemu_state == INT) {
			printf("\n\nUser interrupt\n");
			return;
		}
		else if(is_Break == true) {return;}
		else if(nemu_state == END) { return; }
    }
}

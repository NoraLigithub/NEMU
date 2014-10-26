#include "exec/helper.h"
#include "exec/template-start.h"
#include "cpu/modrm.h"

make_helper(concat(test_r2rm_, SUFFIX)) {
	ModR_M m;
	m.val = instr_fetch(eip + 1, 1);
	if(m.mod == 3){
		unsigned temp = REG(m.reg) & REG(m.R_M);
		cpu.EFLAGS.CF = 0;
		cpu.EFLAGS.OF = 0;
		if(temp == 0) cpu.EFLAGS.ZF = 1;
		else cpu.EFLAGS.ZF = 0;
		if((temp >> 31) & 0x1) cpu.EFLAGS.SF = 1;
		else cpu.EFLAGS.SF = 0;
		print_asm("test" str(SUFFIX) " %%%s,%%%s", REG_NAME(m.R_M),REG_NAME(m.reg));
	return 2;
	}
	else return 0; //temp return 
}

#include "exec/template-end.h"

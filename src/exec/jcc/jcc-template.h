#include "exec/helper.h"
#include "cpu/modrm.h"

make_helper(jcc_je){
	swaddr_t addr = instr_fetch(eip+1,1);
	print_asm("je $0x%x,<main+ox%x>", cpu.eip+2+addr,addr);
	if( cpu.EFLAGS.ZF == 1) cpu.eip = cpu.eip + addr;
	return 2;//temp 2!!
}

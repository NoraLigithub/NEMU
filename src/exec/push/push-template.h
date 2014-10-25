#include "exec/helper.h"
#include "exec/template-start.h"
#include "cpu/modrm.h"

make_helper(concat(push_r2r_,SUFFIX)){
	int reg_code = instr_fetch(eip,1) & 0x7;
	REG(R_ESP)=REG(R_ESP)-DATA_BYTE;
	printf("%x",cpu.esp);
	MEM_W(REG(R_ESP),REG(reg_code));
	print_asm("push" "%%%s",REG_NAME(reg_code));
	return 1;
}

#include "exec/template-end.h"

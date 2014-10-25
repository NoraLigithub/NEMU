#include "exec/helper.h"
#include "exec/template-start.h"
#include "cpu/modrm.h"

make_helper(concat(push_r2r_,SUFFIX)){
	int reg_code = instr_fetch(eip,1) & 0x7;
	REG(4)=REG(4)-4;
	MEM_W(REG(4),REG(reg_code));
	return 1;
}

#include "exec/template-end.h"

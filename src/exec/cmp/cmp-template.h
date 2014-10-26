#include "exec/helper.h"
#include "exec/template-start.h"
#include "cpu/modrm.h"

make_helper(concat(cmp_8i2r_,SUFFIX)) {
	ModR_M m;
	DATA_TYPE imm = instr_fetch(eip + 1 + 1,1);//reg model,not memory
	m.val = instr_fetch(eip+1,1);
	print_asm("cmp" str(SUFFIX) " $0x%x,%%%s", imm, REG_NAME(m.R_M) );
	if(m.mod == 3){
		int temp = REG(m.R_M) - imm;
		cpu.EFLAGS.SF = (temp >> 31) &0x1;
		if(((imm >> 31) & 0x1) && ((REG(m.R_M) >> 31) & 0x1) && !cpu.EFLAGS.SF)			cpu.EFLAGS.OF = 1;
		else if(!((imm >> 31) & 0x1) && !((REG(m.R_M) >> 31) & 0x1) && cpu.EFLAGS.SF)
			cpu.EFLAGS.OF = 1;
		else cpu.EFLAGS.OF  = 0;
		if(REG(m.R_M) > imm) {
			cpu.EFLAGS.ZF = 0;
			cpu.EFLAGS.CF = 0;
		}
		else if(REG(m.R_M) < imm){

			cpu.EFLAGS.ZF = 0;
			cpu.EFLAGS.CF = 1;
		}
		else if(REG(m.R_M ) == imm){
			cpu.EFLAGS.ZF = 1;
			cpu.EFLAGS.CF = 0;
			}
		return 3;
	}
	else return 0; //temp return 
}

#include "exec/template-end.h"

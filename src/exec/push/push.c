#include "exec/helper.h"

#define DATA_BYTE 1
#include "push-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2 
#include "push-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "push-template.h"
#undef DATA_BYTE

extern char suffix;

make_helper(push_r2r_v){
	return (suffix == '1' ? push_r2r_l(eip) : push_r2r_w(eip));
}

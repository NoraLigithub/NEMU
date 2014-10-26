#include "exec/helper.h"

#define DATA_BYTE 1
#include "cmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "cmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "cmp-template.h"
#undef DATA_BYTE

extern char suffix;

make_helper(cmp_8i2r_v) {
	return (suffix == 'l' ? cmp_8i2r_l(eip) : cmp_8i2r_w(eip));
}

#include "ui/breakpoint.h"
#include <stdlib.h>
#include "nemu.h"

#define NR_BP 32

static BP bp_pool[NR_BP];
static BP *head, *free_;

void init_bp_pool() {
	int i;
	for(i = 0; i < NR_BP - 1; i ++) {
		bp_pool[i].NO = i;
		bp_pool[i].next = &bp_pool[i + 1];
	}
	bp_pool[i].next = NULL;

	head = NULL;
	free_ = bp_pool;
}
BP* new_bp()
{/*insert a new node from head*/
	if(free_==NULL) assert(0);
	BP* temp=free_;
	free_=free_->next;
	temp->next=head;
	head=temp;
	return head;
}
void set_bp(char *p){
	int add;
	sscanf(p,"%x",&add);
	BP* new=new_bp();
	new->address=add;
	new->material=swaddr_read(add,1);
	swaddr_write(add,1,INT3_CODE);
	printf("Breakpoint %d at 0x%x\n",(new->NO)+1,add);
}

/* TODO: Implement the function of breakpoint */

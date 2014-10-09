#include "ui/breakpoint.h"
#include <stdlib.h>
#include "nemu.h"

#define NR_BP 32

static BP bp_pool[NR_BP];
static BP *head, *free_;

BP* ret_head(){
	return head;
}
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
void print_bp(){
	BP* temp=head;
	if (temp == NULL)
		printf("No breakpoints.\n");
	else{
		int i;
		for(i=0;i<32;i++){
			temp=head;
			while((temp!=NULL)&&(temp->NO!=i+1))
				temp=temp->next;
			if(temp!=NULL)
				printf("%d\t%x\n",i,temp->address);
		}
	}
}
void free_bp(int number){
	BP* pioneer,*current;
	current=head;
	pioneer=current;
	if(head->NO == number)
		head=head->next;
	else {
		while((current->NO!=number)&&(current->next!=NULL)){
			pioneer=current;
			current=current->next;
		}
		pioneer->next=current->next;
		current->next=free_;
		swaddr_write(current->address,1,current->material);
		free_=current;
	}
}
/* TODO: Implement the function of breakpoint */

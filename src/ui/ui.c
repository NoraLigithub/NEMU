#include "ui/ui.h"
#include "ui/breakpoint.h"
#include "nemu.h"

#include <signal.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int nemu_state = END;

void cpu_exec(uint32_t);
void restart();

/* We use the readline library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

/* This function will be called when you press <C-c>. And it will return to 
 * where you press <C-c>. If you are interesting in how it works, please
 * search for "Unix signal" in the Internet.
 */
static void control_C(int signum) {
	if(nemu_state == RUNNING) {
		nemu_state = INT;
	}
}

void init_signal() {
	/* Register a signal handler. */
	struct sigaction s;
	memset(&s, 0, sizeof(s));
	s.sa_handler = control_C;
	int ret = sigaction(SIGINT, &s, NULL);
	assert(ret == 0);
}

static void cmd_c() {
	if(nemu_state == END) {
		puts("The Program does not start. Use 'r' command to start the program.");
		return;
	}

	nemu_state = RUNNING;
	cpu_exec(-1);
	if(nemu_state != END) { nemu_state = STOP; }
}

static void cmd_r() {
	if(nemu_state != END) { 
		char c;
		while(1) {
			printf("The program is already running. Restart the program? (y or n)");
		    fflush(stdout);
			scanf("%c", &c);
			switch(c) {
				case 'y': goto restart_;
				case 'n': return;
				default: puts("Please answer y or n.");
			}
		}
	}

restart_:
	restart();
	nemu_state = STOP;
	cmd_c();
}

int cmd_p(char *p){
	bool flag=true;
	int result= expr(p,&flag);
	if(flag==false){
		printf("Illegal expression!\n");
		assert(0);
	}
	return result;
}
void main_loop() {
	char *cmd;
	while(1) {
		cmd = rl_gets();
		char *p = strtok(cmd, " ");

		if(p == NULL) { continue; }

		if(strcmp(p, "c") == 0) { cmd_c(); }
		else if(strcmp(p, "r") == 0) { cmd_r(); }
		else if(strcmp(p, "q") == 0) { return; }
		else if(strcmp(p, "si") ==0) {
				if(nemu_state == END) restart();
				nemu_state = RUNNING;
				char *q=strtok(NULL," ");
				int t;
				if (q == NULL) t=1;
				else t=atoi(q);
				cpu_exec(t);
				if(nemu_state!=END)
				nemu_state=STOP;
				}
		else if(strcmp(p,"info")==0){
		char *q=strtok(NULL," ");
		if(strcmp(q,"r")==0){
		printf("eax\t0x%08x\t%10d\n",cpu.eax,cpu.eax);
		printf("ecx\t0x%08x\t%10d\n",cpu.ecx,cpu.ecx);
		printf("edx\t0x%08x\t%10d\n",cpu.edx,cpu.edx);
		printf("ebx\t0x%08x\t%10d\n",cpu.ebx,cpu.ebx);
		printf("esp\t0x%08x\t0x%08x\n",cpu.esp,cpu.esp);
		printf("ebp\t0x%08x\t0x%08x\n",cpu.ebp,cpu.ebp);
		printf("esi\t0x%08x\t%10d\n",cpu.esi,cpu.esi);
		printf("edi\t0x%08x\t%10d\n",cpu.edi,cpu.edi);
		printf("eip\t0x%08x\t0x%08x\n",cpu.eip,cpu.eip);
		}
		else if(strcmp(p,"b") == 0)
			print_bp();
		}    
		
		else if(strcmp(p,"x") == 0){
			char *q=strtok(NULL," ");
			int bite=atoi(q);
			q=strtok(NULL," ");
			int address;
			//sscanf(q,"%x",&address);
			address=cmd_p(q);
			int i=0;
			while(i<bite){
				printf("0x%08x:\t",address+i*4);
				int material=swaddr_read(address+i*4,4);
				printf("0x%08x\n",material);
				i++;
			}
		}
		else if(strcmp(p,"b")==0){
			char *q=strtok(NULL," ");
			if((*q) == '*' && (*(q+1)) == '0' && (*(q+2)) == 'x'){
				int address;
				sscanf(q+3,"%x",&address);
				set_bp(address);
			}
		}
		else if(strcmp(p,"d")==0){
			char* q=strtok(NULL," ");
			if(q==NULL){
				printf("Delete all breakpoints?(y / n)");
				char c;
				scanf("%c",&c);
				if((c == 'y')||(c == 'Y' )){
					int i;
					for( i=0;i<NR_BP;i++)
						free_bp(i);
				}
			}
			else{
				int number=atoi(q);
				if(number>ret_head()->NO)
					printf("No breakpoint number %d.\n",number);
				else free_bp(number);
				free_bp(number);
			}



		}

	 	

		/* TODO: Add more commands */

		else { printf("Unknown command '%s'\n", p); }
	}
}

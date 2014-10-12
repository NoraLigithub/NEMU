#include "common.h"
#include "cpu/reg.h"
#include "memory.h"
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>

enum {
	NOTYPE = 256, EQ, HEX, NUM, REG,LE,GE,LS,RS,NE,AND,OR,INDEX,NEG

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				 // white space
	{"\\+", '+'},					 // plus
	{"==", EQ},	                     // equal
    {"-", '-'},                      // minus
	{"\\*", '*'},                    // multiply
	{"/", '/'},                      // divide
	{"^0x[0-9|a-f|A-F]+", HEX},      // hexadecimol number
	{"[0-9]+", NUM},                 // decimal number
	{"^\\$e[abcdis][px]", REG},      // REG
	{"%", '%'},                      // mod
	{"<<", LS},                      // Left shift
	{">>", RS},                      // right shift
	{"<=", LE},                      // less or equal
	{">=", GE},                      // greater or equal
	{"<", '<'},                      // less
	{">", '>'},                      // greater
	{"!=", NE},                      // not 
	{"&&", AND},                     // logical and
	{"\\|\\|",OR},                   // logical or
	{"&",'&'},                       // bitwise and
	{"\\|",'|'},                     // bitwise or
	{"\\^",'^'},                     // bitwise xor	{"~",'~'},                       // bitwise not
    {"\\(",'('},                     // left parentheses
	{"\\)",')'},                     // right parenthese
	{"\\*",INDEX},                   // index
	{"-",NEG}                        // negative number

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			test(0, "regex compilation failed: %s\n%s\n", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		if(nr_token>=32)
			printf("Buffer overflow!");
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match regex[%d] at position %d with len %d: %.*s", i, position, substr_len, substr_len, substr_start);

				position += substr_len;


				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */

				switch(rules[i].token_type) {
					case NOTYPE: break;
					case '+': case '-': case '*': case '/': case EQ: case NUM: case HEX: case REG: case '%': case LS: case RS: case LE: case GE: case '<': case '>': case NE: case AND: case OR: case '!': case '&': case '|': case '^': case '~': case '(': case ')':
					tokens[nr_token].type=rules[i].token_type;
   				    nr_token++;break;
					default: assert(0);
				}

			if(rules[i].token_type == NUM || rules[i].token_type == HEX || rules[i].token_type == REG)
			strcpy(tokens[nr_token-1].str,substr_start);

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

bool check_parentheses(int p,int q){
	int count=0;
	if((tokens[p].type != '(') || (tokens[q].type != ')'))
		return false;
	int i;
	for(i=p+1;i<q;i++){
		if(tokens[i].type == '(') count++;
		else if(tokens[i].type == ')') count--;
		if(!count) return false;
	}
	if(count==0) return true;
	else return false;
}
int precedence(int t){
	switch(t){
		case NUM: case HEX:
			return -1;
		case '!': case '~':
			return 2;
		case '*': case '/':
			return 3;
		case '+':case '-':
			return 4;
		case LS: case RS: 
	    	return 5;
		case GE: case LE: case '<': case '>':
			return 6;
		case EQ: case NE:
			return 7;
		case '&': return 8;
		case '^': return 9;
	   	case '|': return 10;
		case AND: return 11;
		case OR: return 12;
		default: return -1;
	}
}

int	dominant_op(int p,int q){
	int count=0;
	int index=p;
	int max=-1,temp;
	if(p>q) assert(0);
	while(p<=q){
		if(tokens[p].type=='(') count++;
		else if(tokens[p].type==')') count--;
		if(count<0) {printf("parentheses have errors"); assert(0);} 
		//Wrong parentheses
	    if(count!=0){
			temp=precedence(tokens[p].type);
			if(temp>=max) { max=temp; index=p;}	
		}
		p++; 
	}
	return index;
}

int eval(int p,int q){
	if(p>q){
		printf("Invalid expression!\n");
		assert(0);
		return 0;
	}
	else if(p==q){
		int temp;
		if(tokens[p].type == NUM){
			sscanf(tokens[p].str,"%d",&temp);
			return temp;
		}
		else if(tokens[p].type == HEX){
			sscanf(tokens[p].str,"%x",&temp);
			return temp;
		}
		else if(tokens[p].type == REG){
			if(tokens[p].str[0]!='$' || tokens[p].str[1]!='e'){
				assert(0);
				return 0;
			}
			switch (tokens[p].str[2]){
				case 'a':return cpu.eax;
				case 'c':return cpu.ecx;
				case 'i':return cpu.eip;
				case 'd':{
						 switch(tokens[p].str[3]){
							 case 'x':return cpu.edx;
							 case 'i':return cpu.edi;
						 }
						 }
				case 'b':{
						 switch(tokens[p].str[3]){
							 case 'x':return cpu.ebx;
							 case 'p':return cpu.ebp;
						 }
						 }
				case 's':
						 switch(tokens[p].str[3]){
							 case 'p':return cpu.esp;
							 case 'i':return cpu.esi;
						 }
				default :{ assert(0);return 0;}
			}
		}
	}
	else if(p<q){
		int index=dominant_op(p,q);
		int val2=eval(index+1,q),val1=0;
		if(tokens[index].type!='!' && tokens[index].type!='~' && tokens[index].type!=INDEX && tokens[index].type!=NEG){
			val1=eval(p,index-1);
		}
		switch(tokens[index].type){
			case '+':return val1+val2;
			case '-':return val1-val2;
			case '*':return val1*val2;
			case '/':return val1/val2;
			case '%':return val1%val2;
			case '<':return val1<val2;
			case '>':return val1>val2;
			case EQ:return val1==val2;
			case LE:return val1<=val2;
			case GE:return val1>=val2;
			case NE:return val1!=val2;
			case AND:return val1&&val2;
			case OR:return val1||val2;
			case LS:return val1<<val2;
			case RS:return val1>>val2;
			case '&':return val1&val2;
			case '|':return val1|val2;
			case '^':return val1^val2;
			case '!':return !val2;
			case '~':return ~val2;
			case INDEX:return swaddr_read(val2,4);
			case NEG:return -val2;
			default:{assert(0);return 0;}
		}
	}
		else printf("Invalid expression!\n");
		assert(0);
		return 0;
	}
uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;

		return 0;
	}
	int i,t;
	for(i=0;i<nr_token;i++){
		if(i!=0) t=tokens[i-1].type;
		if(tokens[i].type == '*' &&(i==0 ||(t!=NUM && t!=HEX && t!=REG && t!=')' )))
				tokens[i].type=INDEX;
				else if(tokens[i].type == '-' &&(i==0 ||(t!=NUM && t!=HEX && t!=REG && t!=')' )))
				tokens[i].type=NEG;
	}
	return eval(0,nr_token-1);

	/* TODO: Implement code to evaluate the expression. */
}


#include "nemu.h"
#include <stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,
  TK_NEQ, TK_AND, DEREF
  /* TODO: Add more token types */
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"0x[0-9a-fA-F]+", 'x'},   //hex number
  {"\\$[a-x][a-x][a-x]", 'r'},   //reg
  {"[uU]", TK_NOTYPE},    // u or U 
  {"[0-9]+", 'd'},    //number 
  {"\\+", '+'},    // plus
  {"-", '-'},         // minus
  {"\\*", '*'},    // multiply
  {"/", '/'},    // division
  {"\\(", '('},    // left parenthese
  {"\\)", ')'},    // right parenthese
  {"==", TK_EQ},         // equal   
  {"!=", TK_NEQ},         // not equal   
  {"&&", TK_AND},        // and
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[20000];
int nr_token;
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

	    //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */ 

        switch (rules[i].token_type) {
			case 'd': if(substr_len > 32 ){ 
						  printf("The token str is too long.\n");
						  assert(0);
					  } 
					  else{
						  tokens[nr_token].type = 'd';
						  strcpy(tokens[nr_token].str, substr_start);	
					  }
                      break;
			case 'x': if(substr_len > 32 ){ 
						  printf("The token str is too long.\n");
						  assert(0);
					  } 
					  else{
						  unsigned hex = 0;
						  char s[35] = {};
						  tokens[nr_token].type = 'd';
						  sscanf(substr_start + 2, "%x", &hex);
						  sprintf(s,"%u", hex);
						  strcpy(tokens[nr_token].str, s);	
					  }
            		  break;
			case 'r': tokens[nr_token].type = 'd';
					  uint32_t reg_value = 0;
					  char str[5];
					  for(int i = 0; i < 4; ++i){
						  str[i] = substr_start[i];
					  }
					  str[4] = '\0';
					  //Log("substr is %s\n", str);
					  if(!strcmp(str, "$eax")) {reg_value = cpu.eax;}
					  else if(!strcmp(str, "$ecx")){ reg_value = cpu.ecx;}
					  else if(!strcmp(str, "$edx")){ reg_value = cpu.edx;}
					  else if(!strcmp(str, "$ebx")){ reg_value = cpu.ebx;}
					  else if(!strcmp(str, "$esp")){ reg_value = cpu.esp;}
					  else if(!strcmp(str, "$ebp")){ reg_value = cpu.ebp;}
					  else if(!strcmp(str, "$esi")){ reg_value = cpu.esi;}
					  else if(!strcmp(str, "$edi")){ reg_value = cpu.edi;}
					  else if(!strcmp(str, "$eip")){ reg_value = cpu.eip;}
					  else{ 
						  printf("The reg_name is wrong\n");
						  assert(0);	 
					  }
					  char s[35] = {};
					  //Log("0x%x\n", reg_value);
					  sprintf(s,"%u", reg_value);
					  //Log("s = %s\n", s);
					  strcpy(tokens[nr_token].str, s);	
					  break;
			case '+': tokens[nr_token].type = '+';  break;
			case '-': tokens[nr_token].type = '-';  break;
			case '*': tokens[nr_token].type = '*';  break;
			case '/': tokens[nr_token].type = '/';  break;
			case '(': tokens[nr_token].type = '(';  break;
			case ')': tokens[nr_token].type = ')';  break;
			case TK_NOTYPE:  nr_token --;  break;
			case TK_EQ:  tokens[nr_token].type = TK_EQ;  break;
			case TK_NEQ:  tokens[nr_token].type = TK_NEQ;  break;
			case TK_AND:  tokens[nr_token].type = TK_AND;  break;
			default: printf("Wrong token type.\n");  assert(0);
        }
		nr_token ++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}

bool check_parentheses(int p, int q){
	int t = 0;
	for(int i = p; i <= q; ++i){
		if(tokens[i].type == '('){ 
			++t;
		}
		else if(tokens[i].type == ')'){
			 if(t >= 1){
				t--;
			}
			else{
				//printf("The parentheses are not matched between %d and %d\n, in %d.", p, q, i);
				//assert(0);
				return 0;
			}
		}
		//printf("when %d t is %d\n", i, t);
	}
	if(t != 0){	
		//printf("The parentheses between %d and %d are not matched.\n", p, q);
		//assert(0);
		return 0;
	} 
	/*
	 * if(tokens[p].type == '(' && tokens[q].type == ')'){	
		printf("The parentheses between %d and %d are matched.\n", p, q);
		return 1;
	}*/
	else return 1;
} 

int find_op(int p, int q){
	int parentheses = 0;
	int pri = 10;
	int ans = 0;
	for(int i = p; i <= q; ++i){	
		if (parentheses == 0 && (tokens[i].type == TK_AND)){
			if(pri >= 0){
				ans = i;
				pri = 0;
			}
		}
		else if (parentheses == 0 && (tokens[i].type == TK_EQ || tokens[i].type == TK_NEQ)){
			if(pri >= 1){
				ans = i;
				pri = 1;
			}
		}
		else if (parentheses == 0 && (tokens[i].type == '+' || tokens[i].type == '-')){
			if(pri >= 2){
				ans = i;
				pri = 2;
			}
		} 
		else if (parentheses == 0 && (tokens[i].type == '*' || tokens[i].type == '/')){
		 	if(pri >= 3){
				ans = i;
				pri = 3;
			}
		}
	    else if (parentheses == 0 && (tokens[i].type == DEREF)){
			if(pri >= 4){
				ans = i;
				pri = 4;
			}
		}
		else if (tokens[i].type == '('){
			//printf("Left parenthese.\n");
			parentheses ++;
		}
		else if (tokens[i].type == ')'){
			//printf("Right parenthese.\n");
			parentheses --;
		} 
	}
	return ans;
} 

uint32_t eval(int p, int q){
	//printf("eval(%d, %d).\n", p, q);
	if(p > q){
		printf("In function eval p > q.\n");
		assert(0);
		return 0;
	}
	else if(p == q){
		if(tokens[p].type != 'd') {
			printf("The expression is wrong.\n");
			assert(0);
		}
		else {
			uint32_t temp;
			//printf("The number is %s.\n", tokens[p].str);
			sscanf(tokens[p].str, "%u", &temp);
			//temp = atoi(tokens[p].str);
			return temp;
		 }
	}
	else if (check_parentheses(p, q) == 0){	
		printf("The parentheses are not matched.\n");
		assert(0);
		return 0;
	} 
	else if(tokens[p].type == '(' && tokens[q].type == ')' && check_parentheses(p + 1, q - 1))
	{
		return eval(p + 1, q - 1);
 	}
	else{
		int op = find_op(p, q);
		//printf("The op between %d and %d is no.%d\n", p, q, op);	
		int op_type = tokens[op].type;
		if(op_type == DEREF){
			//printf("DEREF\n");
			//printf("%d %d\n", op, q);
			uint32_t address = eval(p + 1, q);
			//printf("Address is 0x%x\n", address);
			return vaddr_read(address, 8);
		}
		uint32_t val1 = eval(p, op - 1);
		uint32_t val2 = eval(op + 1, q);
		switch(op_type){
			case '+': return val1 + val2;
			case '-': return val1 - val2;		  
			case '/': if(val2 == 0){
						  printf("divide 0. The answer may be wrong.\n");
						  return 0xFFFFFFFF;
						  //assert(0);
					  }
					  return val1 / val2;
			case '*': return val1 * val2;
			case TK_EQ:  if(val1 == val2)  return 1;
					     else return 0;
			case TK_NEQ:  if(val1 == val2)  return 0;
						  else return 1;
			case TK_AND: if(val1 && val2)  return 1;
						 else return 0;
			default: assert(0);  return 0;
		}
 	}
}

uint32_t expr(char *e, bool *success) {
	if (!make_token(e)) {
		*success = false;
		return 0;
	}
	
  /* TODO: Insert codes to evaluate the expression. */
  
	for (int i = 0; i < nr_token; i ++) {
	     if (tokens[i].type == '*' && (i == 0 || tokens[i - 1].type == '('|| tokens[i - 1].type == '+' || tokens[i - 1].type == '-' || tokens[i - 1].type == '*' || tokens[i - 1].type == '/' || tokens[i - 1].type == TK_EQ || tokens[i - 1].type == TK_NEQ || tokens[i - 1].type == TK_AND || tokens[i - 1].type == DEREF)) {
			 //printf("DEREF\n");
			 tokens[i].type = DEREF;
		}
	}	 

	return eval(0, nr_token - 1);
} 

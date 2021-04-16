#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
enum {
  TK_NOTYPE = 256, TK_EQ,TK_H,TK_DE,TK_ES,TK_OR,TK_AND,TK_NEQ,TK_NOT

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},   // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},         // equal
  {"-", '-'},            // subtract
  {"\\*", '*'},          // multiply
  {"/", '/'},            // divide
  {"\\(", '('},            // left
  {"\\)", ')'},            // right
  {"0x[0-9a-fA-F]{1,8}", TK_H},     // hex
  {"[0-9]{1,10}", TK_DE},        // decimal
  {"\\$[a-z]+", TK_ES}, // extra
  {"\\|\\|", TK_OR},
  {"&&", TK_AND},
  {"!=", TK_NEQ},
  {"\\!", TK_NOT},
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

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;
  //printf("args=%s\n",e);
  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
	strncpy(tokens[nr_token].str, e + position, substr_len);
	//printf("str=%s\n",tokens[nr_token].str);
	tokens[nr_token].str[substr_len]='\0';
        if(strlen(tokens[nr_token].str)>31){
	printf("Illegal input exists");
        return false;
	break;}
	else{
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
        	case TK_NOTYPE:
			break;
		case '+':
			tokens[nr_token].type='+';
			nr_token++;
			break;
		case TK_EQ:
			tokens[nr_token].type=TK_EQ;
			nr_token++;
			break;
		case '-':
			tokens[nr_token].type='-';
			nr_token++;
			break;
		case '*':
			tokens[nr_token].type='*';
                        nr_token++;
			break;
		case '/':
			tokens[nr_token].type='/';
                        nr_token++;
                        break;
		case '(':
			tokens[nr_token].type='(';
                        nr_token++;
                        break;
		case ')':
			tokens[nr_token].type=')';
                        nr_token++;
                        break;
		case TK_H:
			tokens[nr_token].type=TK_H;
                        nr_token++;
                        break;
		case TK_DE:
			tokens[nr_token].type=TK_DE;
                        nr_token++;
                        break;
		case TK_ES:
			tokens[nr_token].type=TK_ES;
                        nr_token++;
                        break;
		case TK_OR:
			tokens[nr_token].type=TK_OR;
			nr_token++;
			break;
		case TK_AND:
			tokens[nr_token].type=TK_AND;
			nr_token++;
			break;
		case TK_NEQ:
			tokens[nr_token].type=TK_NEQ;
			nr_token++;
			break;
		case TK_NOT:
			tokens[nr_token].type=TK_NOT;
			nr_token++;
			break;
		default:
                        break;
        }
	}
      }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
       }
  }
  return true;
}
bool check_parentheses(int p,int q){
	int i,left=0,right=0;
	for(i=0;i<=q-p;i++){	
		if(*(tokens+p+i)->str=='(')
		left++;
		if(*(tokens+p+i)->str==')')
		right++;}
	if(left!=right){
	return false;
	assert(0);}
	else{
		if(*(tokens+p)->str=='('&&*(tokens+q)->str==')')
		return true;
		else
		return false;}
}
int prove(int c){
	if(c==TK_OR)
	return 1;
	else
	if(c==TK_AND)
	return 2;
	else
	if(c==TK_NEQ||c==TK_EQ)
	return 3;
	if(c=='+'||c=='-')
	return 4;
	else 
	if(c=='*'||c=='/')
	return 5;
	else
	return 6;
}

int find_dominated_op(int p, int q){
	int level = 0;
	int op = -1; 
	int temp=7;
	for(int i = p; i <= q; i++) { 
		if(tokens[i].type=='(') { 
			level++; } 
		else if(tokens[i].type==')') { 
			level--; } 
		if(level==0&&(tokens[i].type=='+'||tokens[i].type=='-'||tokens[i].type=='*'||tokens[i].type=='/'||tokens[i].type==TK_EQ||tokens[i].type==TK_NEQ||tokens[i].type==TK_OR||tokens[i].type==TK_AND||tokens[i].type==TK_NOT)&&tokens[i+1].type!='-'){
			if(prove(tokens[i].type)<=temp){
				temp=prove(tokens[i].type);
				op=i;}
			}
		else if(level==0&&(tokens[i].type=='+'||tokens[i].type=='-'||tokens[i].type=='*'||tokens[i].type=='/'||tokens[i].type==TK_EQ||tokens[i].type==TK_NEQ||tokens[i].type==TK_OR||tokens[i].type==TK_AND||tokens[i].type==TK_NOT)&&tokens[i+1].type=='-'){
			if(prove(tokens[i].type)<=temp){
				temp=prove(tokens[i].type);
				op=i;}
			i++;
			for(;tokens[i].type=='-';)
			i++;	
	}
		else if(level==0&&(tokens[i].type=='+'||tokens[i].type=='-'||tokens[i].type=='*'||tokens[i].type=='/'||tokens[i].type==TK_EQ||tokens[i].type==TK_NEQ||tokens[i].type==TK_OR||tokens[i].type==TK_AND||tokens[i].type==TK_NOT)&&tokens[i+1].type=='*'){
			if(prove(tokens[i].type)<=temp){
				temp=prove(tokens[i].type);
				op=i;}
			i++;
	}
	}
        return op;

}
uint32_t eval(int p, int q) {
    int op;
    uint32_t result, val1, val2;
    if (p > q) {
        /* Bad expression */
	printf("bad expression\n");
	assert(0);
    }
    else if(tokens[p].type=='-')
        return 0-eval(p+1,q);
    else if(tokens[p].type=='*'){
	return vaddr_read(eval(p+1,q),4);}
    else if (p == q) {
        /* Single token.
        * For now this token should be a number.
        * Return the value of the number.
        */
	if (tokens[p].type==TK_H||tokens[p].type==TK_DE) {
		sscanf(tokens[p].str,"%x",&result);
		return result;
    	}
	else if(tokens[p].type==TK_ES){
        for (int i = 0; i < 8; ++i)
        if (strcmp(regsl[i], tokens[p].str+1) == 0)
                return cpu.gpr[i]._32;
        for (int i = 0; i < 8; ++i)
        if (strcmp(regsw[i], tokens[p].str+1) == 0)
                return cpu.gpr[i]._16;
        for (int i = 0; i < 8; ++i)
        if (strcmp(regsb[i], tokens[p].str+1) == 0)
                return cpu.gpr[i]._8[1];
        if (strcmp("eip", tokens[p].str+1) == 0)
                return cpu.eip;
	else
                assert(0);
	}
	else
		assert(0);
	}
    else if (check_parentheses(p, q) == true) {
        /* The expression is surrounded by a matched pair of parentheses.
        * If that is the case, just throw away the parentheses.
        */
        	return eval(p+1,q-1);}
    else {
        /* We should do more things here. */
	op=find_dominated_op(p, q);
	//printf("%d\n",op);
	if(op==0)
	return !eval(op+1,q);
	else{
	val1=eval(p, op - 1);
	val2=eval(op + 1, q);
	switch (tokens[op].type) {
		case '+':
			return val1+val2;
			break;
		case '-':
			return val1-val2;
			break;
		case '*':
			return val1*val2;
			break;
		case '/':
			return val1/val2;
			break;
		case TK_OR:
			return val1||val2;
			break;
		case TK_AND:
			return val1&&val2;
			break;
		case TK_NEQ:
			if(val1==val2)
			return 0;
			else
			return 1;
			break;
		case TK_EQ:
			if(val1==val2)
			return 1;
			else
			return 0;
			break;
		default:
			assert(0);
		}	
	}
}
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  //printf("nr_token=%d\n",nr_token);
  /* TODO: Insert codes to evaluate the expression. */
  return eval(0, nr_token - 1);
}

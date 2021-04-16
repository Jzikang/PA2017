#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

WP *head;

void cpu_exec(uint64_t,WP *Watch);


/* We use the `readline' library to provide more flexibility to read from stdin. */
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

static int cmd_c(char *args) {
  cpu_exec(-1,head);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step few steps",  cmd_si},
  { "info", "Print the register states", cmd_info},
  { "x", "Scanf memory", cmd_x},
  { "p", "BNF answer", cmd_p},
  { "w", "Watchpoint", cmd_w},
  { "d", "Delete watchpoint", cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args){
	char *arg= strtok(NULL, " ");
	int n;
	if(arg==NULL)
	{
		cpu_exec(1,head);		
	}
	else
	{
		sscanf(arg,"%d",&n);
  		cpu_exec(n,head);

	}		
   	return 1;
}

static int cmd_info(char *args){
  char *arg=strtok(NULL, " ");
  if (strcmp(arg, "r") == 0) {
    for (int i = 0; i < 8; ++i) {
      printf("%s:\t0x%8x\t%d\n", regsl[i], cpu.gpr[i]._32, cpu.gpr[i]._32);}
    printf("eip:\t0x%8x\t%d\n",cpu.eip, cpu.eip);
     for (int i=0; i<8; ++i){
      printf("%s:\t0x%8x\t%d\n", regsl[i], cpu.gpr[i]._16, cpu.gpr[i]._16);}
    for (int i=0; i<8; ++i){
    for (int j=0; j<2; ++j){
      printf("%s:\t0x%8x\t%d\n", regsl[i], cpu.gpr[i]._8[j], cpu.gpr[i]._8[j]);}
    }
  }
  	else{
		list_watchpoint();}	
  return 0;
}
static int cmd_x(char *args) {
  char *arg1=strtok(NULL," ");
  char *arg2=strtok(NULL," ");
  
  int len;
  vaddr_t addr;
  sscanf(arg1,"%d",&len);
  if(arg2[0]=='$')
      addr=cmd_p(arg2);
  else
      sscanf(arg2,"%x",&addr);
  for (int j = 0; j <len ; ++j) {
      printf("0x%08x:",addr);
      printf("\t0x%08x",vaddr_read(addr,4));
      for (int i=1;i<=4;i++){
          printf("\t%02x",vaddr_read(addr,1));
          addr++;}
      printf("\n");
  }
  return 0;
}
static int cmd_p(char *args){
	bool *p=false;
	uint32_t answer;
       	answer=expr(args,p);
	printf("0x%08x\n",answer);
	return answer;
}
static int cmd_w(char *args){
	head=set_watchpoint(args);
	return 0;
}
	
static int cmd_d(char *args){
	char *arg=strtok(NULL," ");
	int N;
	sscanf(arg,"%d",&N);
	delete_watchpoint(N);
	return 0;}
void ui_mainloop(int is_batch_mode) {

  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }
  init_wp_pool();
  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }
	//printf("watch->expr=%s\n",watch->expr);

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

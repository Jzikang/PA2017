#include "nemu.h"
#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */

#define MAX_INSTR_TO_PRINT 100000

int nemu_state = NEMU_STOP;

void exec_wrapper(bool);

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n,WP *Watch) {
  if (nemu_state == NEMU_END) {
    printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
    return;
  }
  nemu_state = NEMU_RUNNING;
  
 // WP *Head;
  //Head=Watch;  
  bool print_flag = n < MAX_INSTR_TO_PRINT;

  for (; n > 0; n --) {
    /* Execute one instruction, including instruction fetch,
     * instruction decode, and the actual execution. */
    exec_wrapper(print_flag);


#ifdef DEBUG
            while(Watch){
                       // printf("head->expr=%s\n",head->expr);
                        Watch->new_val=expr(Watch->expr,false);
                        if(Watch->new_val!=Watch->old_val){
                                printf("Hit watchpoint %d at address 0x%08x\n",Watch->NO,expr("$eip",false));
                                printf("expr      = %s\n",Watch->expr);
                                printf("old value = 0x%08x\n",Watch->old_val);
                                printf("new value = 0x%08x\n",Watch->new_val);
                                printf("program paused\n");
                                Watch->old_val=Watch->new_val;
                                nemu_state=NEMU_STOP;
                                break;}
                        else{
                                Watch=Watch->next;}
                   }
	    if(Watch)
	    break;

    /* TODO: check watchpoints here. */

#endif

#ifdef HAS_IOE
    extern void device_update();
    device_update();
#endif

    if (nemu_state != NEMU_RUNNING) { return; }
  }

  if (nemu_state == NEMU_RUNNING) { nemu_state = NEMU_STOP; }
}

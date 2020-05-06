#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

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
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_p(char *args){
	bool success = 1;
	printf("\033[00;37m------------------------------------------------------------------------- \033[0m\n" );
	printf("%u\n", expr(args, &success));
	printf("\033[00;37m------------------------------------------------------------------------- \033[0m\n" );
	return 0;
}

static int cmd_si(char *args){
	int n = 1;
	printf("\033[00;37m------------------------------------------------------------------------- \033[0m\n" );
	if(args != NULL)
		n = atoi(args);
	cpu_exec(n);
	printf("\033[00;37m------------------------------------------------------------------------- \033[0m\n" );
	return 0;
}

static int cmd_info(char *args){
	printf("\033[00;37m------------------------------------------------------------------------- \033[0m\n" );
	if(args[0] == 'r'){
		printf("\033[32m$eax = 0x%08x %010d\n$ecx = 0x%08x %010d\n$edx = 0x%08x %010d\n$ebx = 0x%08x %010d\n$esp = 0x%08x %010d\n$ebp = 0x%08x %010d\n$esi = 0x%08x %010d\n$edi = 0x%08x %010d\n$eip = 0x%08x %010d\n", cpu.eax, cpu.eax, cpu.ecx, cpu.ecx, cpu.edx, cpu.edx, cpu.ebx, cpu.ebx, cpu.esp, cpu.esp, cpu.ebp, cpu.ebp, cpu.esi, cpu.esi, cpu.edi, cpu.edi, cpu.eip, cpu.eip);
		printf("\033[32mZF = %01x\nSF = %01x\nCF = %01x\nOF = %01x\n", cpu.eflags.ZF, cpu.eflags.SF, cpu.eflags.CF, cpu.eflags.OF);
	}
	else if(args[0] == 'w'){	
		print_wp();
	}
	printf("\033[00;37m------------------------------------------------------------------------- \033[0m\n" );
	return 0;
}

static int cmd_x(char *args){
	printf("\033[00;37m------------------------------------------------------------------------- \033[0m\n" );
	int n = atoi(strtok(args, " "));
	int expr;
	sscanf(strtok(NULL, " "), "%x", &expr);
	for(int i = 0; i < n; ++i){
		printf("\033[35m 0x%x: ", expr);
		for(int j = 0; j < 4; ++j){
			printf("\033[33m%02x ", pmem[expr]);
			expr ++;
		}
	    printf("\n");
	}
	printf("\033[00;37m------------------------------------------------------------------------- \033[0m\n" );
	return 0;
}

static int cmd_w(char *args){
	//printf("\033[00;37m------------------------------------------------------------------------- \033[0m\n" );
	WP* wp = new_wp();
	bool success = true;	
	wp->val = expr(args, &success);
	if(success == 0)
		printf("The expression is wrong.\n");
	strcpy(wp->expre, args);
	//printf("\033[00;37m------------------------------------------------------------------------- \033[0m\n" );
	return 0;
}

static int cmd_d(char *args){
	free_wp(atoi(args));
	return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  
  /* TODO: Add more commands*/
  { "p", "find the value", cmd_p},
  { "si", "Single step", cmd_si},
  { "info", "Print the regs or monitors", cmd_info},
  { "x", "Scan the memory", cmd_x},
  { "d", "Delete the watchpoint", cmd_d},
  { "w", "Add a watchpoint", cmd_w}
  

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
	printf("\033[00;37m------------------------------------------------------------------------- \033[0m\n" );
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
	printf("\033[00;37m------------------------------------------------------------------------- \033[0m\n" );
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

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

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

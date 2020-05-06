#include "klib.h"
#include "proc.h"

#define MAX_NR_PROC 4
extern void naive_uload(PCB *pcb, const char *filename);
extern void context_kload(PCB *pcb, void *entry); 
extern void context_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
	//naive_uload(NULL, "/bin/dummy");
	context_uload(&pcb[1], "/bin/hello");
	//context_uload(&pcb[1], "/bin/hello");
	context_uload(&pcb[0], "/bin/pal");
	context_uload(&pcb[2], "/bin/pal");
	context_uload(&pcb[3], "/bin/pal");
	//context_kload(&pcb[0], (void *)hello_fun);
	switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
	//TODO
	//Log("schedule");
	//save the context pointer
	current->cp = prev;
	
	// always select pcb[0] as the new process
	//current = &pcb[0];
	current = (current == &pcb[1] ? &pcb[fg_pcb] : &pcb[1]);
	/*if(current == &pcb[0]) {
		current = &pcb[fg_pcb];
	}	
	else {
		current = &pcb[0];
	}	
	else if(current == &pcb[2]) {
		current = &pcb[3];
	}	
	else {
		current = &pcb[0];
	}*/	

	//Log("current eip = %x\n", current->cp->eip);
	// then return the new context
	return current->cp;
}

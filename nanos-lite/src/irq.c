#include "klib.h"
#include "common.h"
extern _Context* do_syscall(_Context *c);
extern _Context* schedule(_Context *prev);
extern void _yield();

static _Context* do_event(_Event e, _Context* c) {
	switch (e.event) {
		case _EVENT_YIELD:	
			//Log("event yield recognized."); 
		    //Log("eip = %x\n", c->eip);	
			return schedule(c);
			break;
		case _EVENT_SYSCALL:	
			//Log("event syscall recognized."); 
			return do_syscall(c); 
			break;
		case _EVENT_IRQ_TIMER:	
			Log("event IRQ_TIMER recognized."); 
			_yield();
			break;
		default: panic("Unhandled event ID = %d", e.event);
	}

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}

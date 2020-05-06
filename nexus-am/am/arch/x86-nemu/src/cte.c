#include <am.h>
#include <x86.h>
#include <klib.h>

extern void _switch(_Context *c);
extern void get_cur_as(_Context *c);

static _Context* (*user_handler)(_Event, _Context*) = NULL;

void vectrap();
void vecnull();
void vecsys();
void vectimer();

_Context* irq_handle(_Context *tf) {
	get_cur_as(tf);
  _Context *next = tf;
  if (user_handler) {
    _Event ev;
	//printf("irq = %x\n", tf->irq);
	//printf("eflags = %x\n", tf->eflags);
	//printf("err = %x irq = %x cs = %x \n eax = %x ecx = %x edx = %x ebx = %x\n esp = %x ebp = %x esi = %x edi = %x\n", tf->err, tf->irq, tf->cs, tf->eax, tf->ecx, tf->edx, tf->ebx, tf->esp, tf->ebp, tf->esi, tf->edi);
    switch (tf->irq) {
      case 0x81: ev.event = _EVENT_YIELD; break;
      case 0x80: ev.event = _EVENT_SYSCALL; break;
	  case 0x20: ev.event = _EVENT_IRQ_TIMER; break;
      default: ev.event = _EVENT_ERROR; break;
     }

    next = user_handler(ev, tf);
    if (next == NULL) {
      next = tf;
     }
   }
	_switch(next);
  return next;
} 

static GateDesc idt[NR_IRQ];

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), vecnull, DPL_KERN);
  }

  // -------------------- system call --------------------------
	idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), vectrap, DPL_KERN);
	idt[0x80] = GATE(STS_TG32, KSEL(SEG_KCODE), vecsys, DPL_KERN);
	idt[0x20] = GATE(STS_TG32, KSEL(SEG_KCODE), vectimer, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return 0;
} 

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
	//TODO
	_Context * context = (_Context *)(stack.end - sizeof(_Context));
	memset(context, 0, sizeof(context));
	context->cs = 8;
	context->eflags = 2;
	context->eip = (uintptr_t)(*entry);
	context->ebp = (uintptr_t)(stack.end);
	//printf("entry %x *entry %x\n", (uint32_t)(entry), (uint32_t)(*entry));
	//current->cp = context;
	*((uintptr_t*)stack.start) = (uintptr_t)context;
	return context;
}

void _yield() {
  asm volatile("int $0x81");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}

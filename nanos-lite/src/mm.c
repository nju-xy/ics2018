#include "memory.h"
#include "proc.h"

extern uintptr_t _end;

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t new_brk) {
	//Log("mm_brk");
	if(current->cur_brk == 0) 
		current->cur_brk = current->max_brk;
	//Log("max_brk = %x, cur_brk = %x, new_brk = %x, _end = %x", current->max_brk, current->cur_brk, new_brk, _end);
	while(current->max_brk < new_brk) {
		void *pa = new_page(1);
		void *va = (void *)current->max_brk;
		_map((_Protect *)(&current->as), va, pa, 1);
		current->max_brk += PGSIZE;
		cur_va = current->max_brk;
	}
	current->cur_brk = new_brk;
	return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  //Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);	
}

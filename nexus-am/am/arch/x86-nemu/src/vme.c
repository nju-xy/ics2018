#include <x86.h>
#include <klib.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*pgalloc_usr)(size_t);
static void (*pgfree_usr)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);

  return 0;
}

int _protect(_Protect *p) {
  PDE *updir = (PDE*)(pgalloc_usr(1));
  p->pgsize = 4096;
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
  return 0;
}

void _unprotect(_Protect *p) {
}

static _Protect *cur_as = NULL;
void get_cur_as(_Context *c) {
  c->prot = cur_as;
}

void _switch(_Context *c) {
  set_cr3(c->prot->ptr);
  cur_as = c->prot;
}

int _map(_Protect *p, void *va, void *pa, int mode) {
	//printf("_map\n");
	//printf("va = %x, pa = %x\n", (uint32_t)va, (uint32_t)pa);
	uint32_t paddr = (uint32_t)pa;
	uint32_t vaddr = (uint32_t)va;
	//printf("vaddr = %x, paddr = %x\n", vaddr, paddr);
	uint32_t dir = PDX(vaddr);
	uint32_t page = PTX(vaddr);
	PDE *pde = (PDE *)((uint32_t)p->ptr);
	if((pde[dir] & 0x1) == 0) {//no such pte
		pde[dir] = (PDE)pgalloc_usr(1) | 0x1;
		//printf("pgalloc_usr, pde[dir] = %x\n", pde[dir]);
	}
	assert((pde[dir] & 0x1) == 1);
	//printf("pde[%x] = %x\n", dir, pde[dir]);
	PTE *pte = (PTE *)(PTE_ADDR(pde[dir]));
	assert((pte[page] & 0x1) == 0);
	pte[page] = 0x1 | paddr;
	return 0;
}

_Context *_ucontext(_Protect *p, _Area ustack, _Area kstack, void *entry, void *args) {
    //TODO
	uint32_t *stack = (uint32_t*) ustack.end;
	stack --;
	*stack = 0;
	stack --;
	*stack = 0;
	stack --;
	*stack = 0;
	stack --;
	*stack = 0;
	stack --;
	_Context * context = (_Context *)(ustack.end - sizeof(_Context) - 16);
	memset(context, 0, sizeof(_Context));
	context->cs = 8;
	context->eflags = 0x202;
	context->eip = (uintptr_t)(entry);
	context->ebp = (uint32_t)ustack.end;
	//printf("eip = %x\n", context->eip);
	context->prot = p;	

	*((uintptr_t*)ustack.start) = (uintptr_t)context;
	return context;
	//return NULL;
}

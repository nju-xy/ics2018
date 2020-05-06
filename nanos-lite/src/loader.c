#include "klib.h"
#include "proc.h"
#include "memory.h"
#include <sys/types.h>

//#define DEFAULT_ENTRY 0x4000000
#define DEFAULT_ENTRY 0x8048000
extern uintptr_t _end;
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();

extern int fs_open(const char *pathname, int flags, int mode);
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern ssize_t fs_write(int fd, const void *buf, size_t len);
extern off_t fs_lseek(int fd, off_t offset, int whence);
extern int fs_close(int fd);
extern size_t fs_filesz(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
	//void *buf = (int *)DEFAULT_ENTRY;
	int fd = fs_open(filename, 0, 0);
	size_t len = fs_filesz(fd);
	//origin_brk = DEFAULT_ENTRY + PGROUNDUP(len);
	//if(cur_va == 0)
	cur_va = DEFAULT_ENTRY;
	Log("loader fd = %d, len = %x, pgsize = %x", fd, len, PGSIZE);	
	//for every page in the program
	for(int i = 0; i <= len / PGSIZE; ++i) {
		//sign for a free physical page
		//Log("new page");
		void *pa = new_page(1);
		void *va = (void *)(DEFAULT_ENTRY + i * PGSIZE);
		//Log("pa = %p, va = %p p->ptr = %x", pa, va, pcb->as.ptr);
		//map the phycical page to virtual address space
		_map((_Protect *)(&pcb->as), va, pa, 1);
		//_map((_Protect *)(0x1d6a000), va, pa, 1);
		//Log("map finished");
		//read into the physical page
		if(i == len / PGSIZE)
			fs_read(fd, pa, len & PGMASK);
		else
			fs_read(fd, pa, PGSIZE);
	}
	fs_close(fd);
	pcb->max_brk = DEFAULT_ENTRY + PGROUNDUP(len);
	return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
	_protect((_Protect *)(&pcb->as));
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);
  
  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}

#include <klib.h>
#include "proc.h"
#include "common.h"
#include "syscall.h"
#include <sys/types.h>

extern uintptr_t _end;
extern void _yield();
extern void _halt(int code);
extern int mm_brk(uintptr_t new_brk);
extern void naive_uload(PCB *pcb, const char *filename);
extern int fs_open(const char *pathname, int flags, int mode);
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern ssize_t fs_write(int fd, const void *buf, size_t len);
extern off_t fs_lseek(int fd, off_t offset, int whence);
extern int fs_close(int fd);
int sys_yield();
int sys_open(const char *path, int flag, mode_t mode);
int sys_write(int fd, const void* buf, size_t len);
int sys_read(int fd, void* buf, size_t len);
int sys_close(int fd);
int sys_lseek(int fd, off_t offset, int whence);
int sys_brk(int new_end);
int sys_execve(const char* filename, char *const argv[], char *const envp[]);
_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
	  case 0: //Log("exit");  
			  _halt(a[1]);
			  //naive_uload(NULL, "/bin/init");	
			  break;
	  case 1: //Log("yield");  
			  c->GPRx = sys_yield(); 
			  break;
	  case 2: //Log("open");  
			  c->GPRx = sys_open((const char*)a[1], a[2], a[3]); 
			  break;
	  case 3: //Log("read");  
			  c->GPRx = sys_read(a[1], (void *)a[2], a[3]);  
			  break;
	  case 4: //Log("write");
			  c->GPRx = sys_write(a[1], (const void *)a[2], a[3]);  
			  //return c;
			  break;
	  case 7: //Log("close");  
			  c->GPRx = sys_yield(); 
			  break;
	  case 8: //Log("lseek");  
			  c->GPRx = sys_lseek(a[1], a[2], a[3]); 
			  break;
	  case 9: //Log("brk");
			  c->GPRx = mm_brk((uintptr_t)a[1]); 
			  //return c;
			  break;
	  case 13: //Log("execve");
			   c->GPRx = sys_execve((const char*)a[1], (char *const *)a[2], (char *const *)a[3]);
			   break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
int sys_yield() {
	_yield();
	return 0;
}
int sys_open(const char *path, int flag, mode_t mode) {
	return fs_open(path, flag, mode);
}
int sys_read(int fd, void *buf, size_t len) {
	return fs_read(fd, buf, len);
}
int sys_close(int fd) {
	return fs_close(fd);
}
int sys_lseek(int fd, off_t offset, int whence) {
	return fs_lseek(fd, offset, whence);
}
int sys_write(int fd, const void* buf, size_t len){
	/*char * buf1 = (char *)buf;
	//if(fd == 1|| fd == 2){ 
	//	for(int i = 0; i <  len; i++){
	//		_putc(*buf1);
	//		buf1++;
	//	}
	//}
	//int count = 0;
	//if(fd >= 3)
		//count = fs_write(fd, buf, len);
	//if(fd == 2)
	//	return -1;
	*/
	return fs_write(fd, buf, len);
	//return count;
}
int sys_brk(int new_end){
	//Log("original end = %x", _end);
	//_end = new_end;
	//Log("new end = %x", _end);
	//return 0;
	return mm_brk((uintptr_t)new_end);	
}
int sys_execve(const char* filename, char *const argv[], char *const envp[]) {
	Log("execve %s", filename);
	naive_uload(NULL, filename);	
	return -1;
}

#include "klib.h"
#include "fs.h"
#include <sys/types.h>
//#include <string.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(const void *buf, size_t offset, size_t len);
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t fs_filesz(int fd);
int fs_open(const char *pathname, int flags, int mode);
ssize_t fs_read(int fd, void *buf, size_t len);
ssize_t fs_write(int fd, const void *buf, size_t len);
off_t fs_lseek(int fd, off_t offset, int whence);
int fs_close(int fd);

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write}, //invalid_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write}, //invalid_write},
  {"/dev/fb", 0, 0, 0, invalid_read, invalid_write},
  {"/proc/dispinfo", 128, 0, 0, invalid_read, invalid_write},
  {"/dev/events", 0, 0, 0, invalid_read, invalid_write},
  {"/dev/tty", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb	
  file_table[3].size = 4 * screen_width() * screen_height();
}

size_t fs_filesz(int fd) {
	//my TODO
	return file_table[fd].size;
}
int fs_open(const char *pathname, int flags, int mode) {
	//my TODO
	//Log("fs_open %s", pathname);
	for(int i = 0; i < 64; ++i) {
		if(strcmp(file_table[i].name, pathname) == 0) {
			file_table[i].open_offset = file_table[i].disk_offset;
			//Log("find file %d", i);	
			//Log("origin offset is %x", file_table[i].disk_offset);
			return i;
		}
	}
	//Log("file %s not found", pathname);
	assert(0);
	return 0;
}
ssize_t fs_read(int fd, void *buf, size_t len) {
	//my TODO
	if(fd == 4) { 
		if (len + file_table[fd].open_offset > file_table[fd].disk_offset + file_table[fd].size)
			len = file_table[fd].size + file_table[fd].disk_offset- file_table[fd].open_offset;
		assert(0 <= len + file_table[fd].open_offset);
		assert(len + file_table[fd].open_offset <= file_table[fd].disk_offset + file_table[fd].size);
		dispinfo_read(buf, file_table[fd].open_offset, len);
		file_table[fd].open_offset += len;
		return len;
	}

	if(fd == 5) {
		int ret = events_read(buf, file_table[fd].open_offset, len);
		file_table[fd].open_offset += ret;
		return ret;	
	}

	if (len + file_table[fd].open_offset > file_table[fd].disk_offset + file_table[fd].size)
		len = file_table[fd].size + file_table[fd].disk_offset- file_table[fd].open_offset;
	assert(0 <= len + file_table[fd].open_offset);
    assert(len + file_table[fd].open_offset <= file_table[fd].disk_offset + file_table[fd].size);
	ssize_t ret = ramdisk_read(buf, file_table[fd].open_offset, len);
	file_table[fd].open_offset += len;
	return ret;
}
ssize_t fs_write(int fd, const void *buf, size_t len) {
	//my TODO
	if(fd == 3) {
		int ret = fb_write(buf, file_table[fd].open_offset, len);
		file_table[fd].open_offset += ret;
		return ret;
	}
	if(file_table[fd].write != NULL) {
		return file_table[fd].write(buf, fd, len);
	}
	if (len + file_table[fd].open_offset > file_table[fd].disk_offset + file_table[fd].size)
		len = file_table[fd].size + file_table[fd].disk_offset- file_table[fd].open_offset;
	assert(len + file_table[fd].open_offset <= file_table[fd].disk_offset + file_table[fd].size);
	assert(0 <= len + file_table[fd].open_offset);
	ssize_t ret = ramdisk_write(buf, file_table[fd].open_offset, len);
	file_table[fd].open_offset += len;
	return ret;
}
off_t fs_lseek(int fd, off_t offset, int whence) {
	//my TODO
	//Log("fs_lseek %d", fd);
	//Log("fs_lseek 0x%x 0x%x", offset, whence);
	//Log("offset before is 0x%x 0x%x 0x%x", file_table[fd].open_offset, file_table[fd].disk_offset, file_table[fd].size);
	if(whence == SEEK_CUR)
		file_table[fd].open_offset += offset;
	else if(whence == SEEK_SET)
		file_table[fd].open_offset = file_table[fd].disk_offset + offset;
	else if(whence == SEEK_END)
		file_table[fd].open_offset = file_table[fd].disk_offset + file_table[fd].size + offset;
	if(file_table[fd].open_offset > file_table[fd].disk_offset + file_table[fd].size) { 
		file_table[fd].open_offset = file_table[fd].disk_offset + file_table[fd].size;
	}
	if(file_table[fd].open_offset < file_table[fd].disk_offset) {
		file_table[fd].open_offset = file_table[fd].disk_offset;
	}
	assert(file_table[fd].open_offset >= file_table[fd].disk_offset);
	assert(file_table[fd].open_offset <= file_table[fd].disk_offset + file_table[fd].size);
	//Log("offset now is 0x%x", file_table[fd].open_offset);
	return file_table[fd].open_offset - file_table[fd].disk_offset;
}
int fs_close(int fd) {
	//my TODO
	//Log("fs_close");
	return 0;
}

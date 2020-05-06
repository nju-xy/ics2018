#include "common.h"
#include <amdev.h>
#include "klib.h"
#include <sys/types.h>

extern int fs_open(const char *pathname, int flags, int mode);
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern ssize_t fs_write(int fd, const void *buf, size_t len);
extern off_t fs_lseek(int fd, off_t offset, int whence);
extern int fs_close(int fd);
extern size_t fs_filesz(int fd);
//extern void _yield();

size_t serial_write(const void *buf, size_t offset, size_t len) {
	//_yield();
	int ret = 0;
	char *buf1 = (char*)buf;
	//Log("serial_write %s", buf1);
	for(int i = 0; i < len; ++i) {
		_putc(buf1[i]);
		ret++;
	}
	if(offset == 2)  return -1;
	return ret;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
	//_yield();
	//Log("events_read %d\n", len);
	int key = read_key();
	if(key) {
		if(!(key & 0x8000)) {
			if(keyname[key][0] == 'F') {
				if(keyname[key][1] == '1') {
					fg_pcb = 0;
					//Log("F1");
				}
				if(keyname[key][1] == '2') {
					fg_pcb = 2;
					//Log("F2");
				}
				if(keyname[key][1] == '3') {
					fg_pcb = 3;
					//Log("F3");
				}
			}
		}
		if(key & 0x8000)
			return snprintf(buf, len, "kd %s\n", keyname[key ^ 0x8000]);
		//printf("ku %s", key);
		return snprintf(buf, len, "ku %s\n", keyname[key]);
	}
	//printf("uptime: t %u\n", uptime());
	int ret = snprintf(buf, len, "t %u\n", uptime());
	//printf("buf : %s\n", buf);
	return ret;
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
	//Log("dispinfo_read %d %d", offset, len);
	memcpy(buf, dispinfo + offset, len);	
	//Log("%s", buf);
	//Log("return %d", strlen(buf));
	return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
	//_yield();
	//Log("%d %d", offset, len);
	offset /= 4;
	int x = offset % screen_width();
	int y = offset / screen_width();
	int w = len / 4;//screen_width();
	int h = 1;//screen_width();
	//Log("draw_rect x: %d y: %d w: %d h: %d", x, y, w, h);
	draw_rect((uint32_t*)buf, x, y, w, h);
	return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
	
	//int fd = fs_open("/proc/dispinfo", 0, 0);
	//fs_read(fd, dispinfo, 0);
	sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", screen_width(), screen_height());
	//Log("%s", dispinfo);
	//fs_close(fd);
}

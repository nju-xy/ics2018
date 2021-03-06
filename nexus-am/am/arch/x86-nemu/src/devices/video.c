#include <am.h>
#include <x86.h>
#include <amdev.h>
#include <klib.h>
static uint32_t* const fb __attribute__((used)) = (uint32_t *)0x40000;
#define N 32

size_t video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _VideoInfoReg *info = (_VideoInfoReg *)buf;
	  info->width = 400;
      info->height = 300;
      return sizeof(_VideoInfoReg);
    }
  }
  return 0;
}

size_t video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _FBCtlReg *ctl = (_FBCtlReg *)buf;
	   
	  /*int i;
	  int size = screen_width() * screen_height();
	  for (i = 0; i < size; i ++) fb[i] = i;*/
	  for(int i = 0; i < ctl->h; ++i){
		  for(int j = 0; j < ctl->w; ++j){
		      *(fb + (i + ctl->y) * screen_width() + j + ctl->x) = *(ctl->pixels + j + i * ctl->w);
		  }
	  }
      if (ctl->sync) {
        // do nothing, hardware syncs.
      }
      return sizeof(_FBCtlReg);
    }
  }
  return 0;
}

void vga_init() {
}

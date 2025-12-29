#include <am.h>
#include <amdev.h>
#include <nemu.h>

size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      uint32_t screen_info = inl(SCREEN_ADDR);
      info->width = screen_info >> 16;
      info->height = screen_info & 0xFFFF;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
      
      if (ctl->pixels != NULL) {
        uint32_t *fb = (uint32_t *)FB_ADDR;
        int x = ctl->x, y = ctl->y;
        int w = ctl->w, h = ctl->h;
        uint32_t *pixels = ctl->pixels;
        int screen_width = inl(SCREEN_ADDR) >> 16;
        
        for (int i = 0; i < h; i++) {
          for (int j = 0; j < w; j++) {
            fb[(y + i) * screen_width + (x + j)] = pixels[i * w + j];
          }
        }
      }
      
      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
}

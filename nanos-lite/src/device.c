#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  const char *p = (const char *)buf;
  for (size_t i = 0; i < len; i++) {
    _putc(p[i]);
  }
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  // Check keyboard first (priority)
  int key = read_key();
  if (key != _KEY_NONE) {
    int keydown = (key & 0x8000) != 0;
    int keycode = key & 0x7fff;
    return snprintf(buf, len, "k%c %s\n", keydown ? 'd' : 'u', keyname[keycode]);
  }
  
  // Return timer event if no key event
  return snprintf(buf, len, "t %d\n", uptime());
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  size_t dispinfo_len = strlen(dispinfo);
  if (offset >= dispinfo_len) return 0;
  if (offset + len > dispinfo_len) len = dispinfo_len - offset;
  memcpy(buf, dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int w = screen_width();
  int pixel_offset = offset / sizeof(uint32_t);
  int x = pixel_offset % w;
  int y = pixel_offset / w;
  int npixels = len / sizeof(uint32_t);
  draw_rect((uint32_t *)buf, x, y, npixels, 1);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // Initialize dispinfo with screen dimensions
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}

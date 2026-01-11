#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

// Device I/O function declarations (defined in device.c)
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t fbsync_write(const void *buf, size_t offset, size_t len);

// Ramdisk function declarations (defined in ramdisk.c)
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
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
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
  {"/dev/events", 0, 0, 0, events_read, invalid_write},
  {"/proc/dispinfo", 128, 0, 0, dispinfo_read, invalid_write},
  {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
  {"/dev/fbsync", 0, 0, 0, invalid_read, fbsync_write},
  {"/dev/tty", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < NR_FILES; i++) {
    if (strcmp(file_table[i].name, pathname) == 0) {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  panic("File not found: %s", pathname);
  return -1;
}
size_t fs_read(int fd, void *buf, size_t len) {
  assert(fd >= 0 && fd < NR_FILES);
  Finfo *f = &file_table[fd];
  if (f->read != NULL) {
    return f->read(buf, f->open_offset, len);
  }
  // Regular file: use ramdisk
  size_t remaining = f->size - f->open_offset;
  if (len > remaining) len = remaining;
  ramdisk_read(buf, f->disk_offset + f->open_offset, len);
  f->open_offset += len;
  return len;
}
size_t fs_write(int fd, const void *buf, size_t len) {
  assert(fd >= 0 && fd < NR_FILES);
  Finfo *f = &file_table[fd];
  if (f->write != NULL) {
    return f->write(buf, f->open_offset, len);
  }
  // Regular file: use ramdisk
  size_t remaining = f->size - f->open_offset;
  if (len > remaining) len = remaining;
  ramdisk_write(buf, f->disk_offset + f->open_offset, len);
  f->open_offset += len;
  return len;
}
size_t fs_lseek(int fd, size_t offset, int whence) {
  assert(fd >= 0 && fd < NR_FILES);
  Finfo *f = &file_table[fd];
  size_t new_offset;
  switch (whence) {
    case SEEK_SET: new_offset = offset; break;
    case SEEK_CUR: new_offset = f->open_offset + offset; break;
    case SEEK_END: new_offset = f->size + offset; break;
    default: panic("Invalid whence: %d", whence); return -1;
  }
  if (new_offset > f->size) new_offset = f->size;
  f->open_offset = new_offset;
  return new_offset;
}
int fs_close(int fd) {
  return 0;  // Always succeed
}

void init_fs() {
  // Initialize the size of /dev/fb
  for (int i = 0; i < NR_FILES; i++) {
    if (strcmp(file_table[i].name, "/dev/fb") == 0) {
      file_table[i].size = screen_width() * screen_height() * sizeof(uint32_t);
      break;
    }
  }
}

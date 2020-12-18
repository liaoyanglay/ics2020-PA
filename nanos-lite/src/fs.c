#include <fs.h>
#include <common.h>

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENTS, FD_DISPINFO, FD_FB};

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
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_EVENTS] = {"/dev/events", 0, 0, events_read, invalid_write},
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
#include "files.h"
};

static size_t FT_SIZE = sizeof(file_table) / sizeof(Finfo);

void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = cfg.width * cfg.height * sizeof(uint32_t);
}

int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < FT_SIZE; i++) {
    Finfo *finfo = &file_table[i];
    if (strcmp(pathname, finfo->name) == 0) {
      finfo->open_offset = 0;
      return i;
    }
  }
  panic("Cannot find file '%s'", pathname);
  return 0;
}

size_t fs_read(int fd, void *buf, size_t len) {
  if (fd < 0 || fd >= FT_SIZE) return -1;
  Finfo *finfo = &file_table[fd];
  if (finfo->read) {
    len = finfo->read(buf, finfo->open_offset, len);
    finfo->open_offset += len;
    return len;
  }
  size_t remain = finfo->size > finfo->open_offset ? finfo->size - finfo->open_offset : 0;
  len = len < remain ? len : remain;
  len = ramdisk_read(buf, finfo->disk_offset + finfo->open_offset, len);
  finfo->open_offset += len;
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  if (fd < 0 || fd >= FT_SIZE) return -1;
  Finfo *finfo = &file_table[fd];
  if (finfo->write) {
    len = finfo->write(buf, finfo->open_offset, len);
    finfo->open_offset += len;
    return len;
  }
  size_t remain = finfo->size > finfo->open_offset ? finfo->size - finfo->open_offset : 0;
  len = len < remain ? len : remain;
  len = ramdisk_write(buf, finfo->disk_offset + finfo->open_offset, len);
  finfo->open_offset += len;
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  if (fd < 0 || fd >= FT_SIZE) return -1;
  if (fd == FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR ||
      fd == FD_EVENTS || fd == FD_DISPINFO) {
    panic("File '%s' don't support lseek", file_table[fd].name);
  }
  Finfo *finfo = &file_table[fd];
  switch (whence) {
    case SEEK_SET:
      finfo->open_offset = offset;
      break;
    case SEEK_CUR:
      finfo->open_offset += offset;
      break;
    case SEEK_END:
      finfo->open_offset = finfo->size + offset;
      break;
    default:
      return -1;
  }
  return finfo->open_offset;
}

int fs_close(int fd) {
  if (fd < 0 || fd >= FT_SIZE) return -1;
  return 0;
}

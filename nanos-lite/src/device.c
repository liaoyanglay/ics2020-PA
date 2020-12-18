#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for (char *ch = (char *) buf; ch < (char *) buf + len; ch++) {
    putch(*ch);
  }
  return len;
}

static char event_buf[64];

size_t events_read(void *buf, size_t offset, size_t len) {
  size_t ev_len = strlen(event_buf);
  if (ev_len == 0) {
    // read one event at a time
    AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
    int keydown = ev.keydown;
    int keycode = ev.keycode;
    if (keycode == AM_KEY_NONE) return 0;
    
    event_buf[0] = 'k';
    event_buf[1] = keydown ? 'd' : 'u';
    event_buf[2] = ' ';
    strcpy(&event_buf[3], keyname[keycode]);
    ev_len = strlen(event_buf);
    event_buf[ev_len++] = '\n';
    event_buf[ev_len] = '\0';
  }
  if (ev_len <= len) {
    // read the last event
    strcpy((char *) buf, event_buf);
    strcpy(event_buf, "");
    return ev_len;
  }
  return 0;
}

static char *info_fmt = "WIDTH: %d\nHEIGHT: %d\n";

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  snprintf((char *) buf, len, info_fmt, cfg.width, cfg.height);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  offset = offset / sizeof(uint32_t);
  len = len / sizeof(uint32_t);
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int x = offset % cfg.width;
  int y = offset / cfg.width;
  size_t remain = cfg.width - x;
  len = len < remain ? len : remain;
  io_write(AM_GPU_FBDRAW, x, y, (void *) buf, len, 1, true);
  return len * sizeof(uint32_t);
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}

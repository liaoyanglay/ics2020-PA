#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;
static int canvas_x = 0, canvas_y = 0;

uint32_t NDL_GetTicks() {
  struct timeval time;
  gettimeofday(&time, NULL);
  return time.tv_sec * 1000 + time.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  return read(evtdev, buf, len) > 0 ? 1 : 0;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  } else {
    if (*w == 0 && *h == 0) {
      *w = screen_w;
      *h = screen_h;
    }
    assert(*w <= screen_w);
    assert(*h <= screen_h);
    canvas_w = *w;
    canvas_h = *h;
    canvas_x = (screen_w - canvas_w) / 2;
    canvas_y = (screen_h - canvas_h) / 2;
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  assert(x + w <= canvas_w);
  assert(y + h <= canvas_h);
  for (int i = 0; i < h; i++) {
    lseek(fbdev, ((i + canvas_y + y) * screen_w + canvas_x + x) * sizeof(uint32_t), SEEK_SET);
    write(fbdev, pixels + i * w, w * sizeof(uint32_t));
  }
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  } else {
    if (evtdev == -1) evtdev = open("/dev/events", O_RDONLY);
    if (fbdev == -1) fbdev = open("/dev/fb", O_RDWR);
  }

  int disproc = open("/proc/dispinfo", O_RDONLY);
  char buf[64];
  int off = 0;
  read(disproc, buf, sizeof(buf));
  close(disproc);
  while (buf[off++] != '\n') /* empty */;
  assert(strncmp(buf, "WIDTH", 5) == 0);
  sscanf(buf + 5, "%*[: \t] %d", &screen_w);
  assert(strncmp(buf + off, "HEIGHT", 6) == 0);
  sscanf(buf + off + 6, "%*[: \t] %d", &screen_h);

  return 0;
}

void NDL_Quit() {
  close(evtdev);
  close(fbdev);
}

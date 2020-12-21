#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

#define KEY_NUMBER (sizeof(keyname) / sizeof(char *))
static uint8_t key_state[KEY_NUMBER];

int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[64];
  if (NDL_PollEvent(buf, sizeof(buf)) == 0) return 0;
  if (buf[0] == 'k') {
    ev->type = buf[1] == 'd' ? SDL_KEYDOWN : SDL_KEYUP;
    int len = strlen(buf);
    buf[len - 1] = '\0';
    for (int i = 0; i < KEY_NUMBER; i++) {
      if (strcmp(&buf[3], keyname[i]) == 0) {
        ev->key.keysym.sym = i;
        key_state[i] = ev->type == SDL_KEYDOWN;
        return 1;
      }
    }
  }
  assert(0);
  return 0;
}

int SDL_WaitEvent(SDL_Event *ev) {
  char buf[64];
  while (NDL_PollEvent(buf, sizeof(buf)) == 0)
    /* empty */;
  if (buf[0] == 'k') {
    ev->type = buf[1] == 'd' ? SDL_KEYDOWN : SDL_KEYUP;
    for (int i = 0; i < sizeof(keyname) / sizeof(char *); i++) {
      if (strncmp(&buf[3], keyname[i], strlen(keyname[i])) == 0) {
        ev->key.keysym.sym = i;
        key_state[i] = ev->type == SDL_KEYDOWN;
        return 1;
      }
    }
  }
  assert(0);
  return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return key_state;
}

#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t n;
  for (n = 0; *s != '\0'; s++)
    n++;
  return n;
}

char *strcpy(char* dst, const char* src) {
  char *ret = dst;
  while ((*dst++ = *src++) != '\0')
    /* do nothing */;
  return ret;
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t i;
  char *ret = dst;
  for (i = 0; i < n; i++) {
    *dst++ = *src;
    if (*src != '\0')
      src++;
  }
  return ret;
}

char* strcat(char* dst, const char* src) {
  char *ret = dst;
  while (*dst != '\0')
    dst++;
  while ((*dst++ = *src++) != '\0')
    /* do nothing */;
  return ret;
}

int strcmp(const char* s1, const char* s2) {
  while (*s1 && *s1 == *s2)
    s1++, s2++;
  return (int) ((unsigned char) *s1 - (unsigned char) *s2);
}

int strncmp(const char* s1, const char* s2, size_t n) {
  while (n > 0 && *s1 && *s1 == *s2)
    n--, s1++, s2++;
  if (n == 0)
    return 0;
  else
    return (int) ((unsigned char) *s1 - (unsigned char) *s2);
}

void* memset(void* v, int c, size_t n) {
  char *p = v;
  int m = n;
  while (--m >= 0)
    *p++ = c;

  return v;
}

void* memmove(void* dst, const void* src, size_t n) {
  const char *s = src;
  char *d = dst;
  if (s < d && s + n > d) {
    s += n;
    d += n;
    while (n-- > 0)
      *--d = *--s;
  } else {
    while (n-- > 0)
      *d++ = *s++;
  }
  return dst;
}

void* memcpy(void* out, const void* in, size_t n) {
  return memmove(out, in, n);
}

int memcmp(const void* v1, const void* v2, size_t n) {
  const uint8_t *s1 = (const uint8_t *) v1;
  const uint8_t *s2 = (const uint8_t *) v2;

  while (n-- > 0) {
    if (*s1 != *s2)
      return (int) *s1 - (int) *s2;
    s1++, s2++;
  }
  return 0;
}

#endif

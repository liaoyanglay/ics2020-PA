#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

int NDL_Init(uint32_t flags);
uint32_t NDL_GetTicks();
void NDL_Quit();

int main() {
  int i = 1;
  NDL_Init(0);
  uint32_t msec = NDL_GetTicks();
  printf("Time of day: %u\n", msec / 1000);
  while (i < 10) {
    uint32_t new_msec = NDL_GetTicks();
    if (new_msec >= msec + 1000) {
      msec = new_msec;
      printf("Time of day: %u\n", msec / 1000);
      i++;
    }
  }
  NDL_Quit();
  return 0;
}
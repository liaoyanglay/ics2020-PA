#include <am.h>
#include <nemu.h>

static uint64_t boot_sec;
static uint64_t boot_usec;

void __am_timer_init() {
  boot_usec = inl(RTC_ADDR);
  boot_sec = inl(RTC_ADDR + 4);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint64_t usec = inl(RTC_ADDR) - boot_usec;
  uint64_t sec = inl(RTC_ADDR + 4) - boot_sec;
  uptime->us = sec * 1000000 + usec;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}

#ifndef TIME_CONTROL_H
#define TIME_CONTROL_H

#include "Arduino.h"
#include <stdio.h>
#include "I2C_RTC.h"

DS1307 rtc;

int new_time = 0, base_time = 0;

int TIME_SECS = 0;

// setupClock
void setup_timer(){
  if (!rtc.begin()){
    Serial.println("Module thoi gian khong duoc ket noi");
    Serial.flush();
    return;
  }
  rtc.setTime(0, 0, 0);
  rtc.setDate(11, 9, 2001); // đừng hỏi vì sao :)
  rtc.startClock();
}

bool a_seconds(int set_time){
  new_time = rtc.getSeconds();
  if(new_time - base_time >= set_time) {
    rtc.setTime(0, 0, 0);
    return true;
  }
  return false;
}

void tick_timer(){
  if(a_seconds(1)) TIME_SECS += 1;
}

int get_time(){
  return TIME_SECS;
}

#endif
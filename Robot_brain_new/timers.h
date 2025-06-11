#ifndef TIMERS_H
#define TIMERS_H

#include "I2C_RTC.h"

DS1307 rtc;

void setup_timer(){
  if(!rtc.begin()){
    Serial.println("Module thoi gian khong duoc ket noi");
    Serial.flush();
    while(1);
  }
  rtc.setTime(0, 0, 0);
  rtc.setDate(11, 9, 2001); // đừng hỏi vì sao :)
}

#endif
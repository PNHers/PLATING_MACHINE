#ifndef TIME_CONTROL_H
#define TIME_CONTROL_H

#include "Arduino.h"
#include <stdio.h>
#include "I2C_RTC.h"

DS1307 rtc;

int new_time = 0, base_time = 0;

int TIME_SECS = 0;

// setupClock
void initTimer() {
    if (!rtc.begin()) {
        Serial.println("Module thoi gian khong duoc ket noi");
        Serial.flush();
        return;
    }

    rtc.setTime(0, 0, 0);
    rtc.setDate(11, 9, 2001); // đừng hỏi vì sao :)
    rtc.startClock();
}

bool isASecondPassed() {
    new_time = rtc.getSeconds();

    if (new_time - base_time >= 1) {
        rtc.setTime(0, 0, 0);
        return true;
    }

    return false;
}

void updateTime() {
    if (!isASecondPassed()) return;
    TIME_SECS += 1;
}

int getTime() {
    return TIME_SECS;
}

#endif
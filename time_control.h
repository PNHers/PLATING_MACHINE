#ifndef TIME_CONTROL_H
#define TIME_CONTROL_H

#include "Arduino.h"
#include <stdio.h>
#include "I2C_RTC.h"

DS1307 rtc;

int SECONDS_PASSED = 0;

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
    if (rtc.getSeconds() >= 1) {
        rtc.setTime(0, 0, 0);
        return true;
    }

    return false;
}

void updateTime() {
    if (!isASecondPassed()) return;
    SECONDS_PASSED += 1;
}

int getTime() {
    return SECONDS_PASSED;
}

#endif
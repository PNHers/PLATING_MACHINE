#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include "config_button.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void set_servo(uint8_t channel, uint16_t pulse) {
  pwm.setPWM(channel, 0, pulse);
}

void servo_setup(){
  set_servo(SERVO_1_CHANNEL, 205);
  set_servo(SERVO_2_CHANNEL, 205);
  set_servo(SERVO_3_CHANNEL, 205);
  set_servo(SERVO_4_CHANNEL, 205);
  set_servo(SERVO_5_CHANNEL, 205);
  set_servo(SERVO_6_CHANNEL, 205);
}

#endif
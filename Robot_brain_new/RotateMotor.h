#ifndef ROTATEMOTOR_H
#define ROTATEMOTOR_H

#include <stdio.h>
#include <vector>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "config_button.h"
#include "gyro_control.h"

#define MENSURE_VAULE 0.05 // sai số khi joystick để ở vị trí ban đầu
#define max_power 2025
#define STEP 100

// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

using namespace ControlState;

int MAX_LEVEL = MAX_GEAR + MAX_GEAR;
bool already_pull = false, is_rotate = false;

struct Rotate {
    int power;
    int pin1;
    int pin2;
};

// ý là cái hàm này không xài nhưng vẫn giữ lại đây =)) // gan den ngay di thi moi xai :)))))))
void swap(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}

// lưu các giá trị vào ma trận
void Div_level() {
    int temp = (MAX_POWER - MIN_POWER) / MAX_GEAR;
    POWER_LEVEL[0] = MIN_POWER;

    for (int i = 1; i <= MAX_GEAR; i++) POWER_LEVEL[i] = POWER_LEVEL[i - 1] + temp;

    POWER_LEVEL[MAX_GEAR] = MAX_POWER;
    POWER_LEVEL[0] = 0;

    power_lift = temp / 2;
    self_rotate_gap = (MAX_POWER - MIN_POWER) * (SELF_ROTATE_RATIO / 100.0);
}

void smooth_motor(int *left_motor, int *right_motor);

// anh dung dep trai
void setPWMMotors(Rotate motor_info, Adafruit_PWMServoDriver *pwm) {
    if (motor_info.power < 0)
        return;

    pwm->setPin(motor_info.pin1, motor_info.power);
    pwm->setPin(motor_info.pin2, 0);
    delay(50);
    Serial.print(motor_info.power);
    Serial.print(" pin: ");
    Serial.print(motor_info.pin1);
    Serial.print(" ");
    Serial.println(motor_info.pin2);
}

void setServo180(Adafruit_PWMServoDriver *pwm, uint8_t channel, int rotate_angle) {
    // float handled_angle = (rotate_angle - MIN_ANGLE_DEFAULT)/ (MAX_ANGLE_DEFAULT - MIN_ANGLE_DEFAULT);
    uint16_t pulse = (MAX_ROTATE - MIN_ROTATE) * rotate_angle / 180 + MIN_ROTATE;
    pwm->setPWM(channel, 0, 0);
    pwm->setPWM(channel, 0, pulse);
    Serial.print("rotate collector arm at: ");
    Serial.println(pulse);
    pwm->setPWM(channel, 0, 0);
}

void setServo360(Adafruit_PWMServoDriver *pwm, uint8_t channel, int rotate) {
    pwm->setPWM(channel, 0, 0);
    pwm->setPWM(channel, 0, rotate);
    Serial.print("rotate collector at: ");
    Serial.println(rotate);
    delay(20);
    pwm->setPWM(channel, 0, 0);
}

//               pin :8,9,10,11,12,13,14,15
int Motor_speed[8] = {0,0,0 ,0 ,0 ,0 ,0 ,0};

void setPWMMotors2(int *power, PIN *pin) {
    if (*power < 0 || *power > 4096) return;
    int power_current1 = Motor_speed[pin->pin1 - 8];
    // int power_current2 = Motor_speed[pin->pin2];
    if (power_current1 * (*power) == 0 ) {
        
        if (Motor_speed[pin->pin2 - 8] != 0 && (*power) != 0){
            Serial.println("////////////////////////////// error set power wrong //////////////////////////////////////");
            // pwm.setPin(pin->pin1, 0);
            // pwm.setPin(pin->pin2, 0);
            // delay(50);
            *power = 0;
        }
    }
    
    pwm.setPin(pin->pin1, *power);
    pwm.setPin(pin->pin2, 0);
    // delay(50);
    
    // Serial.print(*power);
    // Serial.print(" pin: ");
    // Serial.print(pin->pin1);
    // Serial.print(" ");
    // Serial.println(pin->pin2);

    Motor_speed[pin->pin1 - 8] = *power;
    Motor_speed[pin->pin2 - 8] = 0;
}

// void smooth_motor(int *left_motor, int *right_motor) {
//     left_power = motor_left_smooth.updateEstimate(*left_motor);
//     right_power = motor_right_smooth.updateEstimate(*right_motor);
//     // Serial.print(left_power);
//     // Serial.print(",");
//     // Serial.println(right_power);
//     setPWMMotors2(&left_power, &left_pin);
//     setPWMMotors2(&right_power, &right_pin);
// }

// tu tu roi lam :)
void check_min_power() {
    Serial.println("checking min power");
    delay(1000);

    for (auto const &min_value : MIN_VALUE) {
        Serial.print("Testing ");
        Serial.println(min_value);
        delay(500);

        // chay motor
        if (isRobotMoving())
        {
            MIN_POWER = min_value;
            // dung motor
            break;
        }
        else
        {
            // dung motor
        }

        delay(1000);
    }
}

void motorPowerChangeImmediately(bool is_lift, int& motorPower, int pin1, int pin2, bool is_swap, SimpleKalmanFilter* motor_filter){
    // if (motorPower == 0) return;
    if (is_swap) swap(pin1, pin2);

    motorPower = is_lift ? 4096 : 0;
    motorPower = motor_filter->updateEstimate(motorPower);

    PIN pin = {pin1, pin2};

    setPWMMotors2(&motorPower, &pin);
}

void motorPowerChange(int& motorPower, int pin1, int pin2, bool is_swap, SimpleKalmanFilter* motor_filter){
    // if (motorPower == 0) return;
    if (is_swap) swap(pin1, pin2);

    motorPower = motor_filter->updateEstimate(motorPower);

    PIN pin = {pin1, pin2};

    setPWMMotors2(&motorPower, &pin);
}

void turnWhenMove(){
    if (abs(x_axis) != 1) return;
    if (x_axis > 0) right_power = max(right_power - (left_power * TURN_RATIO[CURRENT_GEAR]) / 100, 0);
    if (x_axis < 0) left_power = max(left_power - (right_power * TURN_RATIO[CURRENT_GEAR]) / 100, 0);
}

void motorControl() {
    using namespace ControlState;
    motorPowerChangeImmediately(is_motor_a, motor_power_A, 12, 13, is_motor_a_reverse,  &motor_A_smooth);
    motorPowerChangeImmediately(is_motor_b, motor_power_B, 14, 15, is_motor_b_reverse,  &motor_B_smooth);

    if(y_axis == 1) left_power = right_power = POWER_LEVEL[CURRENT_GEAR];
    else left_power = right_power = 0;

    if (is_rotate_left || is_rotate_right){
        if (!right_power) is_rotate_right = false;
        if (!left_power) is_rotate_left = false;
    }

    switch (CURRENT_GEAR){
        case 0:
            if(abs(x_axis) == 1) break;
            motorPowerChangeImmediately(false, left_power, 8, 9, is_rotate_left ? !is_motor_left_reverse : is_motor_left_reverse,  &motor_left_smooth);
            motorPowerChangeImmediately(false, right_power, 10, 11, is_rotate_right ? !is_motor_right_reverse : is_motor_right_reverse,  &motor_right_smooth);
            return;
        default:
            turnWhenMove();
            break;
    };

    if (left_power == right_power && left_power == 0){
        if (invert) x_axis *= -1; 
        if(abs(x_axis) == 1){
            if(x_axis < 0){
                right_power = 1024;
                is_rotate_right = true;
                motorPowerChange(right_power, 10, 11, is_motor_right_reverse,  &motor_right_smooth);
            }
            else if (x_axis > 0){
                left_power = 1024;
                is_rotate_left = true;
                motorPowerChange(left_power, 8, 9, is_motor_left_reverse,  &motor_left_smooth);   
            }
            return;
        }
    }

    motorPowerChange(left_power, 8, 9, is_motor_left_reverse,  &motor_left_smooth);
    motorPowerChange(right_power, 10, 11, is_motor_right_reverse,  &motor_right_smooth);
}



#endif
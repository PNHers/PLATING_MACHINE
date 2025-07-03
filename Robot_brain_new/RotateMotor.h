#ifndef ROTATEMOTOR_H
#define ROTATEMOTOR_H

#include <stdio.h>
#include <vector>

#include <Wire.h>
#include "config_button.h"

#include "PCA9685.h"

#define MENSURE_VAULE 0.05 // sai số khi joystick để ở vị trí ban đầu
#define max_power 2025
#define STEP 100

using namespace ControlState;

int MAX_LEVEL = MAX_GEAR + MAX_GEAR;
bool already_pull = false, is_rotate = false;

// servo 2, 3, 4, 5, 6, 7
// motor 8 9, 10 11, 12 13, 14 15

int rotate_motor[8] = {0,0,0,0,0,0,0,0};
int Motor_speed[8] = {0,0,0 ,0 ,0 ,0 ,0 ,0};

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
}

// anh dung dep trai

void setServo180( uint8_t channel, int rotate_angle) {
    uint16_t pulse = (MAX_ROTATE - MIN_ROTATE) * rotate_angle / 180 + MIN_ROTATE;
    pwms[channel] = pulse;
}

void setServo360(uint8_t channel, int rotate) {
    pwms[channel] = rotate;
}

void setPWMMotors2(PIN *pin) {
    int powerpin1 = rotate_motor[pin->pin1 - 8];
    int power = (powerpin1 != 0)?powerpin1 : rotate_motor[pin->pin2 - 8];

    int pin1 = (powerpin1 != 0)?pin->pin1 : pin->pin2;
    int pin2 = (pin->pin1 + pin->pin2) - pin1;

    if (power < 0 || power > 4096) return;
    int power_current1 = Motor_speed[pin1 - 8];
    int power_current2 = Motor_speed[pin->pin2];
    if (power_current1 * (power) == 0 ) {
        
        if (Motor_speed[pin2 - 8] != 0 && (power) != 0){
            Serial.println("////////////////////////////// error set power wrong //////////////////////////////////////");
            power = 0;
        }
    }
    
    // Serial.print(power);
    // Serial.print(" pin: ");
    // Serial.print(pin->pin1);
    // Serial.print(" ");
    // Serial.println(pin->pin2);
    pwms[pin1] = power;
    pwms[pin2] = 0;

    Motor_speed[pin1 - 8] = power;
    Motor_speed[pin2 - 8] = 0;
    
}

void motorPowerChangeImmediately(bool is_lift, int& motorPower, int pin1, int pin2, bool is_swap, SimpleKalmanFilter* motor_filter){
    if (is_swap) swap(pin1, pin2);

    motorPower = is_lift ? 4096 : 0;
    motorPower = motor_filter->updateEstimate(motorPower);

    PIN pin = {pin1, pin2};

    rotate_motor[pin1 - 8] = motorPower;
    rotate_motor[pin2 - 8] = 0;
}

void motorPowerChange(int& motorPower, int pin1, int pin2, bool is_swap, SimpleKalmanFilter* motor_filter){
    if (is_swap) swap(pin1, pin2);

    motorPower = motor_filter->updateEstimate(motorPower);

    PIN pin = {pin1, pin2};

    rotate_motor[pin1 - 8] = motorPower;
    rotate_motor[pin2 - 8] = 0;    
}

void turnWhenMove(){
    if (abs(x_axis) != 1) return;
    if (x_axis > 0) right_power = CURRENT_GEAR ? POWER_LEVEL[CURRENT_GEAR] : POWER_LEVEL[1];
    if (x_axis < 0) left_power = CURRENT_GEAR ? POWER_LEVEL[CURRENT_GEAR] : POWER_LEVEL[1];
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

void rotate_all_thing() {
    setPWMMotors2(new PIN(8,9));
    setPWMMotors2(new PIN(10,11));
    setPWMMotors2(new PIN(12,13));
    setPWMMotors2(new PIN(14,15));
}

void reset_motor(){
    rotate_motor[0] = 0;
    rotate_motor[1] = 0;
    rotate_motor[2] = 0;
    rotate_motor[3] = 0;
    rotate_motor[4] = 0;
    rotate_motor[5] = 0;
    rotate_motor[6] = 0;
    rotate_motor[7] = 0;
}

void safety_check(){
    if (pwms[8] * pwms[9] != 0) pwms[8] = pwms[9] = 0;
    if (pwms[10] * pwms[11] != 0) pwms[10] = pwms[11] = 0;
    if (pwms[12] * pwms[13] != 0) pwms[12] = pwms[13] = 0;
    if (pwms[14] * pwms[15] != 0) pwms[14] = pwms[15] = 0;
}


#endif
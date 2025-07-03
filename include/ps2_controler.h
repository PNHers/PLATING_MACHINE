#ifndef PS2_CONTROLER_H
#define PS2_CONTROLER_H

#include <Wire.h>     //thư viện I2c của Arduino, do PCA9685 sử dụng chuẩn giao tiếp i2c nên thư viện này bắt buộc phải khai báo
#include <PS2X_lib.h> // Khai báo thư viện
#include "HardwareSerial.h"
#include "config_button.h"
#include "RotateMotor.h"
#include "Arduino.h"


// Định nghĩa các chân điều khiển
#define PS2_DAT 12 // MISO
#define PS2_CMD 13 // MOSI
#define PS2_SEL 15 // SS
#define PS2_CLK 14 // SLK

const float sai_so = 0.01; // sai số

int collector_rotation_angle = DEFAULT_ANGLE;
int collector_angle = 0;

int time_base_collector = 0, time_now_collector = 0;
int time_base_hold_fruit = 0;
bool is_double_press = false;
bool is_holding_collector_button = false;
bool is_counting_press_time = false;

bool is_start_fruit_basket = false;
bool is_high_basket = false;

PS2X ps2x; // khởi tạo class PS2x

void initPS2() {
    int error = -1;

    // thử kết nối với tay cầm ps2 trong n lần
    while (true) {
        delay(500);
        // cài đặt chân và các chế độ: GamePad
        error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);
        Serial.print(".");
        if (!error) break;
    }
}

void positionOfJoystick(float &x_axis, float &y_axis) {
    const float HANDLED_PSX = (ps2x.Analog(PSS_RX) - 127.5) / 127.5;
    const float HANDLED_PSY = (ps2x.Analog(PSS_LY) - 127.5) / 127.5;
    x_axis = HANDLED_PSX;
    y_axis = -HANDLED_PSY;
}

void resetMotionState() {
    using namespace ControlState;

    if (!ps2x.Button(GEAR_UP))    is_gear_up = false;
    if (!ps2x.Button(GEAR_DOWN))  is_gear_down = false;
    if (!ps2x.Button(ZERO_FORCE)) is_zero_force = false;
    if (!ps2x.Button(REVERSE))    is_reverse = false;

    if (!ps2x.Button(PSB_R1) && !ps2x.Button(PSB_R2)) {
        is_motor_a = false;
    }
    if (!motor_power_A) is_motor_a_reverse = false;

    if (!ps2x.Button(PSB_PAD_UP) && !ps2x.Button(PSB_PAD_DOWN)) {
        is_motor_b = false;
    }
    if (!motor_power_B) is_motor_b_reverse = false;
}

void consoleRead() {
    using namespace ControlState;

    if (ps2x.Button(GEAR_UP) && !is_gear_up && !is_rotate_left && !is_rotate_right) {
        CURRENT_GEAR += 1;
        CURRENT_GEAR = min(CURRENT_GEAR, MAX_GEAR);
        is_gear_up = true;
    }

    if (ps2x.Button(GEAR_DOWN) && !is_gear_down && !is_rotate_left && !is_rotate_right) {
        CURRENT_GEAR -= 1;
        CURRENT_GEAR = max(CURRENT_GEAR, 0);
        is_gear_down = true;
    }

    if (ps2x.Button(ZERO_FORCE) && !is_zero_force) {
        CURRENT_GEAR = 0;
        fast_stop = true;
        is_zero_force = true;
    }

    if (ps2x.Button(REVERSE) && !is_reverse && !is_rotate_left && !is_rotate_right) {
        motorPowerChangeImmediately(false, left_power, 8, 9, is_rotate_left ? !is_motor_left_reverse : is_motor_left_reverse,  &motor_left_smooth);
        motorPowerChangeImmediately(false, right_power, 10, 11, is_rotate_right ? !is_motor_right_reverse : is_motor_right_reverse,  &motor_right_smooth);
        fast_stop = true;
        invert = is_motor_right_reverse = is_motor_left_reverse = !invert;
        is_reverse = true;
    }

    if (ps2x.Button(PSB_R1) && !is_motor_a && !motor_power_A){
        is_motor_a = true;
    }

    if (ps2x.Button(PSB_R2) && !is_motor_a && !motor_power_A){
        is_motor_a = true;
        is_motor_a_reverse = true;
    }

    if (ps2x.Button(PSB_PAD_UP) && !is_motor_b && !motor_power_B){
        is_motor_b = true;
    }

    if (ps2x.Button(PSB_PAD_DOWN) && !is_motor_b && !motor_power_B){
        is_motor_b = true;
        is_motor_b_reverse = true;
    }

    if (y_axis == 1 && !is_motor_left && !is_motor_right && !left_power && !right_power){
        is_motor_left = true;
        is_motor_right = true;
    }

    if (y_axis == 0 && is_motor_left && is_motor_right){
        is_motor_left = false;
        is_motor_right = false;
    }
}

void controlCollector() {

    if (ps2x.ButtonPressed(PSB_GREEN)) collector_rotation_angle += 1; 
    else if (ps2x.ButtonPressed(PSB_BLUE)) collector_rotation_angle -= 1;
    
    if (ps2x.Button(PSB_GREEN) || ps2x.Button(PSB_BLUE)) {
        if (!is_holding_collector_button) {
            time_base_collector = millis();
            is_holding_collector_button = true;
        }

        time_now_collector = millis();

        if (time_now_collector - time_base_collector >= TIME_SET_COLLECTOR_CHANGE_SPEED) {
            collector_rotation_angle += (ps2x.Button(PSB_GREEN)) ? 1 : -1;
        }
    }


    is_holding_collector_button = (!ps2x.Button(PSB_GREEN) && !ps2x.Button(PSB_BLUE)) ? false : is_holding_collector_button;

    collector_rotation_angle = min(collector_rotation_angle, 180);
    collector_rotation_angle = max(collector_rotation_angle, 0);

    setServo180( COLLECTOR_ROTATION_PIN, collector_rotation_angle);

    int collector_angle = 0;

    if (ps2x.ButtonReleased(PSB_PINK)) {
        is_counting_press_time = true;
        time_base_hold_fruit = millis();
    }

    if (ps2x.Button(PSB_RED)) {
        collector_angle = POWER_OPEN_COLLECTOR;
    } else if (ps2x.Button(PSB_PINK)) {
        if (is_double_press) {
            collector_angle = POWER_HOLD_COLLECTOR;
        } else {
            collector_angle = POWER_CLOSE_COLLECTOR;
        }
    }

    if (is_counting_press_time) {
        if (millis() - time_base_hold_fruit >= TIME_SET_COLLECTOR_CHANGE_SPEED) {
            is_counting_press_time = false;
            is_double_press = false;
        } else if (ps2x.Button(PSB_PINK)) {
            is_double_press = true;
            is_counting_press_time = false;
        }
    }

    setServo360( COLLECTOR_PIN, collector_angle);

    // control fruit basket
    if (ps2x.Button(PSB_SELECT)) is_start_fruit_basket = true;
    if (!ps2x.Button(PSB_SELECT) && is_start_fruit_basket) {
        pwms[BASKET_CONTROL_PIN] = 0;
        is_start_fruit_basket = false;
    }
    
    if (ps2x.Button(PSB_START)) is_high_basket = true;
    if (!ps2x.Button(PSB_START) && is_high_basket) {
        pwms[BASKET_CONTROL_PIN] = 0;
        is_high_basket = false;
    }

    if (is_high_basket) {setServo180( BASKET_CONTROL_PIN, BASKET_HIGH_DEFAULT_ROTATION);}
    else if (is_start_fruit_basket) {setServo180( BASKET_CONTROL_PIN, BASKET_DEFAULT_ROTATION);}
}

#endif
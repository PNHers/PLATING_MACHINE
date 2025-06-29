#include "HardwareSerial.h"
#ifndef PS2_CONTROLER_H
#define PS2_CONTROLER_H

#include <Wire.h>     //thư viện I2c của Arduino, do PCA9685 sử dụng chuẩn giao tiếp i2c nên thư viện này bắt buộc phải khai báo
#include <PS2X_lib.h> // Khai báo thư viện
#include "config_button.h"

// Định nghĩa các chân điều khiển
#define PS2_DAT 12 // MISO
#define PS2_CMD 13 // MOSI
#define PS2_SEL 15 // SS
#define PS2_CLK 14 // SLK

const float sai_so = 0.01; // sai số

int collector_rotation_angle = DEFAULT_ANGLE;

bool Is_hold_collector_button = false;
int time_base_collector = 0, time_now_collector = 0;
int time_base_hold_fruit = 0;
bool Is_counting_press_time = false;
bool Is_press_twice = false;



PS2X ps2x; // khởi tạo class PS2x

// hàm này loại trừ sai số của tay cầm
void check(float *x_ps2, float *y_ps2) { 
    if (*x_ps2 <= sai_so && *x_ps2 >= -sai_so)
        *x_ps2 = 0.0f;
    if (*y_ps2 <= sai_so && *y_ps2 >= -sai_so)
        *y_ps2 = 0.0f;
}

void rotate(float x_ps2, float y_ps2, float *x_axis, float *y_axis) {
    check(&x_ps2, &y_ps2); // kiểm tra sai số
    float length = sqrt(x_ps2 * x_ps2 + y_ps2 * y_ps2);
    float actually_x_axis = x_ps2, actually_y_axis = y_ps2;

    // check nếu 2 giá trị được nhập từ console lớn hơn 1 || -1 (có sai số khi test);
    if (x_ps2 > 1)
        actually_x_axis = 1;
    else if (x_ps2 < -1)
        actually_x_axis = -1;
    if (y_ps2 > 1)
        actually_y_axis = 1;
    else if (y_ps2 < -1)
        actually_y_axis = -1;

    float cos_a = x_ps2 / length;

    if (length <= 1.00)
    {
        if (x_ps2 >= 0)
        {
            actually_x_axis = length * cos_a;
            actually_y_axis = sqrt(length * length - actually_x_axis * actually_x_axis);
        }
        else
        {
            actually_y_axis = -(length * cos_a);
            if (y_ps2 == 0)
                actually_y_axis = 0;
            actually_x_axis = -sqrt(length * length - actually_y_axis * actually_y_axis);
        }
    }
    else
    {
        if (x_ps2 >= 0 || abs(y_ps2) == 1)
        {
            actually_x_axis = 1.00 * cos_a;
            actually_y_axis = sqrt(1 - actually_x_axis * actually_x_axis);
        }
        else
        {
            actually_x_axis = 1.00 * cos_a;
            actually_y_axis = sqrt(1 - actually_x_axis * actually_x_axis);
        }
    }

    if (y_ps2 < 0)
        actually_y_axis *= -1;

    // chỗ này không có thì kết quả cuối ra giá trị nan
    if (x_ps2 == 0)
        actually_x_axis = 0;
    if (y_ps2 == 0)
        actually_y_axis = 0;

    // Serial.print(actually_x_axis);
    // Serial.print(" ");
    // Serial.println(actually_y_axis);

    // length = sqrt(x_ps2*x_ps2 + y_ps2*y_ps2);
    // if(length > 1) length = 1;
    // left_motor = length, right_motor = length;

    // cos_a = actually_x_axis / length;

    // if (y_ps2 < 0) {
    //   left_motor *= -1; right_motor *= -1;

    //   if (cos_a < 0){
    //     float right_move = (-cos_a - 0.5)*(-2);
    //     right_motor *= right_move;
    //   }
    //   else if (cos_a > 0) {
    //     float left_move = (cos_a - 0.5)*(-2);
    //     left_motor *= left_move;
    //   }
    // }
    // else{
    //   // adding somthign
    //   if (cos_a > 0){
    //     float right_move = (cos_a - 0.5)*(-2);
    //     right_motor *= right_move;
    //   }
    //   else if (cos_a < 0) {
    //     float left_move = (-cos_a - 0.5)*(-2);
    //     left_motor *= left_move;
    //   }
    // }

    // Serial.print(left_motor);
    // Serial.print(" : ");
    // Serial.println(right_motor);

    *x_axis = actually_x_axis;
    *y_axis = actually_y_axis;
}

void initPS2() {
    int error = -1;
    while (true) // thử kết nối với tay cầm ps2 trong n lần
    {
        delay(500); // đợi 1 giây
        // cài đặt chân và các chế độ: GamePad
        error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);
        Serial.print(".");
        if (!error) // kiểm tra nếu tay cầm đã kết nối thành công
            break;  // thoát khỏi vòng lặp
    }
}

void positionOfJoystick(float &x_axis, float &y_axis) {
    ps2x.read_gamepad(); // gọi hàm để đọc tay điều khiển
    const float HANDLED_PSX = (ps2x.Analog(PSS_RX) - 127.5) / 127.5;
    const float HANDLED_PSY = (ps2x.Analog(PSS_LY) - 127.5) / 127.5;
    // rotate(handled_psx, -handled_psy, x_axis, y_axis);
    x_axis = HANDLED_PSX;
    y_axis = -HANDLED_PSY;
}

void smooth_joystick() {
    x_axis = joystick_x_stab.updateEstimate(console_x_axis);
    y_axis = joystick_y_stab.updateEstimate(console_y_axis);
}

void resetMotionState() {
    using namespace ControlState;

    if (!ps2x.Button(GEAR_UP)) {
        is_gear_up = false;
    } if (!ps2x.Button(GEAR_DOWN)) {
        is_gear_down = false;
    } if (!ps2x.Button(ZERO_FORCE)) {
        is_zero_force = false;
    } if (!ps2x.Button(REVERSE)) {
        is_reverse = false;
    }
}

void consoleRead() {
    using namespace ControlState;

    if (ps2x.Button(GEAR_UP) && !is_gear_up && !is_rotate_left && !is_rotate_right) {
        CURRENT_GEAR += 1;
        if (CURRENT_GEAR > MAX_GEAR)
            CURRENT_GEAR = MAX_GEAR;
        Serial.println(CURRENT_GEAR);
        is_gear_up = true;
    }

    if (ps2x.Button(GEAR_DOWN) && !is_gear_down && !is_rotate_left && !is_rotate_right) {
        CURRENT_GEAR -= 1;
        if (CURRENT_GEAR < 0)
            CURRENT_GEAR = 0;
        Serial.println(CURRENT_GEAR);
        is_gear_down = true;
    }

    if (ps2x.Button(ZERO_FORCE) && !is_zero_force) {
        CURRENT_GEAR = 0;
        FAST_MOTOR_STOP();
        fast_stop = true;
        Serial.println(CURRENT_GEAR);
        is_zero_force = true;
    }

    if (ps2x.Button(REVERSE) && !is_reverse) {
        FAST_MOTOR_STOP();
        fast_stop = true;
        CURRENT_GEAR = 0;
        invert = !invert;
        Serial.println("reverse mode");
        is_reverse = true;
    }
}

void control_collector(Adafruit_PWMServoDriver* pwm){
  if (ps2x.Button(PSB_GREEN) || ps2x.Button(PSB_BLUE)){
    if (!Is_hold_collector_button){time_base_collector = millis();}
    time_now_collector = millis();

    if (time_now_collector - time_base_collector > TIME_SET_COLLECTOR_CHANGE_SPEED){
      collector_rotation_angle += (ps2x.Button(PSB_GREEN))? 1 : -1;
    }
  }

  if (ps2x.ButtonPressed(PSB_GREEN)){
    Is_hold_collector_button = true;
    collector_rotation_angle += 1;
  }
  else if (ps2x.ButtonPressed(PSB_BLUE)){
    Is_hold_collector_button = true;
    collector_rotation_angle -= 1;
  }

  Is_hold_collector_button = (ps2x.ButtonReleased(PSB_GREEN) || ps2x.ButtonReleased(PSB_BLUE))? false : Is_hold_collector_button;



  if (collector_rotation_angle > 180) {collector_rotation_angle = 180;}
  else if (collector_rotation_angle < 0) {collector_rotation_angle = 0;}

  setServo180(pwm, COLLECTOR_ROTATION_PIN, collector_rotation_angle);

  int collector_angle = 0;

  if (ps2x.ButtonReleased(PSB_PINK)){
    if (!Is_press_twice){
      Is_counting_press_time = true;
      time_base_hold_fruit = millis();
    }
    else {Is_press_twice = false;}
  }

  if (ps2x.Button(PSB_RED)){
    collector_angle = POWER_OPEN_COLLECTOR;
  }
  else if (ps2x.Button(PSB_PINK)){
    if (Is_press_twice){collector_angle = POWER_HOLD_COLLECTOR;}
    else {collector_angle = POWER_CLOSE_COLLECTOR;}
  }

  if (Is_counting_press_time){
    if (millis() - time_base_hold_fruit > TIME_SET_COLLECTOR_CHANGE_SPEED){
      Is_counting_press_time = false;
    }
    else if (ps2x.Button(PSB_PINK)){
      Is_press_twice = true;
    }
  }

  setServo360(pwm, COLLECTOR_PIN, collector_angle);
  
}


#endif
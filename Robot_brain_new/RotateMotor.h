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
    POWER_LEVEL[LEFT][MAX_GEAR] = POWER_LEVEL[RIGHT][MAX_GEAR] = MIN_POWER;

    for (int i = 1; i <= MAX_GEAR; i++) {
        POWER_LEVEL[LEFT][i + MAX_GEAR] = POWER_LEVEL[LEFT][i + MAX_GEAR - 1] + temp;
        POWER_LEVEL[RIGHT][i + MAX_GEAR] = POWER_LEVEL[RIGHT][i + MAX_GEAR - 1] + temp;
        MOTOR_PIN[LEFT][i + MAX_GEAR] = {LEFT_PIN_1, LEFT_PIN_2};
        MOTOR_PIN[RIGHT][i + MAX_GEAR] = {RIGHT_PIN_1, RIGHT_PIN_2};
    }

    int k = MAX_GEAR + MAX_GEAR;
    for (int i = 0; i < MAX_GEAR; i++) {
        POWER_LEVEL[LEFT][i] = POWER_LEVEL[LEFT][k];
        POWER_LEVEL[RIGHT][i] = POWER_LEVEL[RIGHT][k];
        k--;
        MOTOR_PIN[LEFT][i] = {LEFT_PIN_2, LEFT_PIN_1};
        MOTOR_PIN[RIGHT][i] = {RIGHT_PIN_2, RIGHT_PIN_1};
    }

    POWER_LEVEL[LEFT][0] = POWER_LEVEL[RIGHT][0] = MAX_POWER;
    POWER_LEVEL[LEFT][MAX_GEAR + MAX_GEAR] = POWER_LEVEL[RIGHT][MAX_GEAR + MAX_GEAR] = MAX_POWER;
    POWER_LEVEL[LEFT][MAX_GEAR] = POWER_LEVEL[RIGHT][MAX_GEAR] = 0;

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
// làm motor dừng từ từ nhưng nhanh
void instantSmoothBrake() {
    left_pin = MOTOR_PIN[LEFT][MAX_GEAR + 1];
    right_pin = MOTOR_PIN[RIGHT][MAX_GEAR + 1];

    if (ControlState::invert) {
        left_pin = MOTOR_PIN[LEFT][MAX_GEAR - 1];
        right_pin = MOTOR_PIN[RIGHT][MAX_GEAR - 1];
    }

    if(ControlState::invert) Serial.println("he he he");

    while (new_power_left != 0 || new_power_right != 0) {
        new_power_left  = max(new_power_left  - power_lift, 0);
        new_power_right = max(new_power_right - power_lift, 0);
        smooth_motor(&new_power_left, &new_power_right);
        delay(10);
    }
}

// luôn đi đúng tốc độ // kiểu như là đang đi hết ga số 3 thì lùi về số 2 chắc chắn xe sẽ giảm tốc cho phù hợp với số 2
void correctGearSpeed() {
    if (!(new_power_left > POWER_LEVEL[LEFT][MAX_GEAR + CURRENT_GEAR] ||
          new_power_right > POWER_LEVEL[RIGHT][MAX_GEAR + CURRENT_GEAR])) return;

    new_power_left  = min(new_power_left  - power_lift, POWER_LEVEL[LEFT][MAX_GEAR  + CURRENT_GEAR]);
    new_power_right = min(new_power_right - power_lift, POWER_LEVEL[RIGHT][MAX_GEAR + CURRENT_GEAR]);

    delay(100);
}

void move2() {
    //  Serial.println(CURRENT_GEAR);
    if (ControlState::invert && CURRENT_GEAR > 0)
        CURRENT_GEAR *= -1;

    left_pin = MOTOR_PIN[LEFT][MAX_GEAR + CURRENT_GEAR];
    right_pin = MOTOR_PIN[RIGHT][MAX_GEAR + CURRENT_GEAR];

    if (robot_status == 1) {
        new_power_left  = min(new_power_left  + (int)(power_lift * y_axis), POWER_LEVEL[LEFT][MAX_GEAR + CURRENT_GEAR]);
        new_power_right = min(new_power_right + (int)(power_lift * y_axis), POWER_LEVEL[RIGHT][MAX_GEAR + CURRENT_GEAR]);
        delay(100);
    }

    if (robot_status == -1) {
        new_power_left  = max(new_power_left  + (int)(power_lift * y_axis), 0);
        new_power_right = max(new_power_right + (int)(power_lift * y_axis), 0);
        delay(50);
    }

    correctGearSpeed();

    if (abs(x_axis) > when_to_rotate) {
        if (x_axis > 0) {
            new_power_right -= power_lift * x_axis;

            if (new_power_left - new_power_right > max_different_rotate)
                new_power_right = new_power_left - max_different_rotate;

            new_power_right = max(new_power_right, 0);
        }

        if (x_axis < 0) {
            new_power_left -= power_lift * -x_axis;

            if (new_power_right - new_power_left > max_different_rotate)
                new_power_left = new_power_right - max_different_rotate;
            
            new_power_left = max(new_power_left, 0);
        }
    } else {
        if (new_power_left > new_power_right) {
            new_power_right = min(new_power_right + power_lift, new_power_left);
        } else if (new_power_left < new_power_right) {
            new_power_left = min(new_power_left + power_lift, new_power_right);
        }
    }

    if (ControlState::invert && CURRENT_GEAR < 0)
        CURRENT_GEAR *= -1;
}

void self_rotate() {
    using namespace ControlState;

    if (fast_stop) return;

    correctGearSpeed();
    left_pin = MOTOR_PIN[LEFT][MAX_GEAR + 1];
    right_pin = MOTOR_PIN[RIGHT][MAX_GEAR + 1];

    int delta_x_power = self_rotate_gap * x_axis;

    if (abs(x_axis) > when_to_rotate) {
        if (x_axis > 0 && !is_rotate_left) {
            // new_power_left  = min(new_power_left  + delta_x_power, self_rotate_gap);
            new_power_right = min(new_power_right + delta_x_power, self_rotate_gap);
            right_pin = MOTOR_PIN[RIGHT][MAX_GEAR - 1];
            is_rotate_right = true;
        }

        if (x_axis < 0 && !is_rotate_right) {
            new_power_left  = min(new_power_left  - delta_x_power, self_rotate_gap);
            // new_power_right = min(new_power_right - delta_x_power, self_rotate_gap);
            left_pin = MOTOR_PIN[LEFT][MAX_GEAR - 1];
            is_rotate_left = true;
        }

        if (x_axis > 0 && is_rotate_left) {
            // new_power_left  = max(new_power_left  - self_rotate_gap, 0);
            new_power_right = max(new_power_right - self_rotate_gap, 0);
            left_pin = MOTOR_PIN[LEFT][MAX_GEAR - 1];
        }

        if (x_axis < 0 && is_rotate_right) {
            new_power_left  = max(new_power_left  - self_rotate_gap, 0);
            // new_power_right = max(new_power_right - self_rotate_gap, 0);
            right_pin = MOTOR_PIN[RIGHT][MAX_GEAR - 1];
        }
    } else if (is_rotate_right || is_rotate_left) {
        new_power_left  = max(new_power_left  - self_rotate_gap, 0);
        new_power_right = max(new_power_right - self_rotate_gap, 0);
        
        if (is_rotate_right) {
            right_pin = MOTOR_PIN[RIGHT][MAX_GEAR - 1];
        } else if (is_rotate_left) {
            left_pin = MOTOR_PIN[LEFT][MAX_GEAR - 1];
        }
    }

    if (!(is_rotate_left || is_rotate_right) || !(new_power_left == new_power_right) || !(0 == new_power_left)) return;
    is_rotate_left = false;
    is_rotate_right = false;
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

void smooth_motor(int *left_motor, int *right_motor) {
    left_power = motor_left_smooth.updateEstimate(*left_motor);
    right_power = motor_right_smooth.updateEstimate(*right_motor);
    // Serial.print(left_power);
    // Serial.print(",");
    // Serial.println(right_power);
    setPWMMotors2(&left_power, &left_pin);
    setPWMMotors2(&right_power, &right_pin);
}

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

void motorPowerChange(bool is_lift, int& motorPower, int pin1, int pin2, bool is_swap, SimpleKalmanFilter* motor_filter){
    // if (motorPower == 0) return;
    if (is_swap) swap(pin1, pin2);

    motorPower = is_lift ? 4096 : 0;
    motorPower = motor_filter->updateEstimate(motorPower);

    PIN pin = {pin1, pin2};

    setPWMMotors2(&motorPower, &pin);
}

void motorControl() {
    using namespace ControlState;
    motorPowerChange(is_motor_a, motor_power_A, 12, 13, is_motor_a_reverse,  &motor_A_smooth);
    motorPowerChange(is_motor_b, motor_power_B, 14, 15, is_motor_b_reverse,  &motor_B_smooth);
}


void move3(){ //hẹ hẹ hẹ

}

#endif
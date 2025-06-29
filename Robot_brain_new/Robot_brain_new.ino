#include "time_control.h"
#include "RotateMotor.h"
#include "ps2_controler.h"
#include "gyro_control.h"
#include "config_button.h"
#include <SimpleKalmanFilter.h>

// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// int BASE_TIME = 0 , TIME = 0, NEW_TIME = 0, DEM = 0;

void checkStatus(float y_axis) {
    if (ControlState::invert) {
        y_axis *= -1;
    }

    if (abs(y_axis) < IDLE) {
        robot_status = HOLD;
    } else if (y_axis > 0) {
        robot_status = PULL;
    } else if (y_axis < 0) {
        robot_status = BACK;
    }
}

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("Initializing systems...");
    delay(2000);

    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(50);
    Wire.setClock(400000);

    initPS2();
    // initGyro();
    // check_min_power();
    Serial.println("test");
    Div_level();

    initTimer();
    // // gyro_setup_tick();
    // gyro_tick = 1.0 / myIMU.getGyroDataRate();
    // Serial.println(myIMU.getGyroDataRate());
    Serial.println("Initialation success!");
}

void loop() {
    using namespace ControlState;
    // // tick_timer();
    // get_accel();
    ps2x.read_gamepad();

    controlCollector(&pwm);

    // consoleRead();
    // positionOfJoystick(console_x_axis, console_y_axis);
    // checkStatus(console_y_axis);

    // smoothenJoystick();

    // // Serial.print(console_y_axis);
    // // Serial.print(",");
    // // Serial.print(console_x_axis);
    // // Serial.print(",");
    // // Serial.print(y_axis);
    // // Serial.print(",");
    // // Serial.println(x_axis);

    // max_different_rotate = POWER_LEVEL[LEFT][MAX_GEAR + CURRENT_GEAR] * (TURN_RATIO / 100.0);

    // if (CURRENT_GEAR == 0)
    //     self_rotate();
    // else if (!fast_stop && !is_rotate_left && !is_rotate_right)
    //     move2();

    // new_power_left = max(0, new_power_left);
    // new_power_right = max(0, new_power_right);

    // // RotateInfo left_motor = {&current_power_left, &new_power_left, left_pin.pin1, left_pin.pin2};
    // // RotateInfo right_motor = {&current_power_right, &new_power_right, right_pin.pin1, right_pin.pin2};

    // // rotate_2_motor(left_motor, right_motor, &pwm);

    // smooth_motor(&new_power_left, &new_power_right);

    // current_power_left = new_power_left;
    // current_power_right = new_power_right;

    // // Serial.print(current_power_left);
    // // Serial.print(" ");
    // // Serial.println(current_power_right);
    // if (fast_stop) fast_stop = false;
    // resetMotionState();

    // Serial.print(A_X);
    // Serial.print(",");
    // Serial.print(A_Y);
    // Serial.print(",");
    // Serial.println(A_Z);

    // Serial.print(GYRO_X);
    // Serial.print(",");
    // Serial.print(GYRO_Y);
    // Serial.print(",");
    // Serial.print(GYRO_Z);
    // Serial.print(",");

    // Serial.print(ROLL);
    // Serial.print(",");
    // Serial.print(PITCH);
    // Serial.print(",");
    // Serial.println(YAW);

    // if(detect_movement()) Serial.println("Object is moving!");

    // delay(5);
}

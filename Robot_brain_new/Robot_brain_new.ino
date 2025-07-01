#include "time_control.h"
#include "RotateMotor.h"
#include "ps2_controler.h"
#include "gyro_control.h"
#include "config_button.h"
#include <SimpleKalmanFilter.h>

// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// int BASE_TIME = 0 , TIME = 0, NEW_TIME = 0, DEM = 0;

TaskHandle_t Task0;

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

    // initTimer();
    // // gyro_setup_tick();
    // gyro_tick = 1.0 / myIMU.getGyroDataRate();
    // Serial.println(myIMU.getGyroDataRate());

    GyroSettings::oldVelocityTime = millis();
    xTaskCreatePinnedToCore(
                xTask0,   /* Task function. */
                "Gyroscope, accel & time",     /* name of task. */
                10000,       /* Stack size of task */
                NULL,        /* parameter of the task */
                1,           /* priority of the task */
                &Task0,      /* Task handle to keep track of created task */
                0);          /* pin task to core 0 */   

    Serial.println("Initialation success!");
}

void xTask0( void * pvParameters ){
    while(true){
        // get_accel();
        // calculateVelocity();
        // Serial.println(GyroSettings::velocity);
        // Serial.println(GyroSettings::accel_x);
        controlCollector(&pwm);
        delay(1);
    }
}

int time_base = 0;
int time_play = 0;

void loop() {
    time_base = millis();
    
    using namespace ControlState;
    // // tick_timer();
    // get_accel();
    ps2x.read_gamepad();

    // controlCollector(&pwm);


    positionOfJoystick(console_x_axis, console_y_axis);
    checkStatus(console_y_axis);
    smoothenJoystick();
    consoleRead();

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

    motorControl();
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
    if (fast_stop && !current_power_left && !current_power_right) fast_stop = false;
    resetMotionState();


    // if(detect_movement()) Serial.println("Object is moving!");

    delay(1);
    
    time_play = millis() - time_base;
    Serial.println(time_play);
}

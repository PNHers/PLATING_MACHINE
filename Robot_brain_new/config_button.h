#ifndef CONFIG_BUTTON_H
#define CONFIG_BUTTON_H

#include <vector>
#include <SimpleKalmanFilter.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// CONSOLE

#define GEAR_UP PSB_L2
#define GEAR_DOWN PSB_L1
#define ZERO_FORCE PSB_L3
#define REVERSE PSB_R3

// MOTOR SETTING

#define HOLD 0
#define PULL 1
#define BACK -1

#define IDLE 0.1 // stand

int MIN_POWER = 0;
#define MAX_POWER 2048
#define MAX_GEAR 4
#define MAX_ROTATE_SPEED 3 // giá trị này phải bé hơn MAX_GEAR
#define when_to_rotate 0.2 // khi nào robot nên xoay
#define CHANGE_PULL 1
#define BRAKE 3
#define TURN_RATIO 40        // %
#define SELF_ROTATE_RATIO 30 // %

#define LEFT 0
#define RIGHT 1

#define LEFT_PIN_1 8 // đầu dương
#define LEFT_PIN_2 9
#define RIGHT_PIN_1 10 // đầu dương
#define RIGHT_PIN_2 11

// SERVO SETTING

#define SERVO_1_CHANNEL 2
#define SERVO_2_CHANNEL 3
#define SERVO_3_CHANNEL 4
#define SERVO_4_CHANNEL 5
#define SERVO_5_CHANNEL 6
#define SERVO_6_CHANNEL 7

#define NOTIFY_LED 13

#define MIN_ROTATE 80
#define MAX_ROTATE 570

#define DEFAULT_ANGLE 90
#define MIN_ANGLE_DEFAULT 0
#define MAX_ANGLE_DEFAULT 180
#define COLLECTOR_PIN 3
#define COLLECTOR_ROTATION_PIN 2

#define POWER_OPEN_COLLECTOR 100
#define POWER_CLOSE_COLLECTOR 500
#define POWER_HOLD_COLLECTOR 347
#define TIME_SET_HOLD_FRUIT 600
#define TIME_SET_COLLECTOR_CHANGE_SPEED 600

SimpleKalmanFilter joystick_x_filter(0.5, 0.01, 0.8);
SimpleKalmanFilter joystick_y_filter(0.5, 0.01, 0.8);

SimpleKalmanFilter motor_left_smooth(1, 1, 1);
SimpleKalmanFilter motor_right_smooth(1, 1, 1);

/*/////////////////////////////////////////////////////

All below here are temp variable for processing

*/
/////////////////////////////////////////////////////

// STRUCT
struct PIN {
    int pin1;
    int pin2;
};

struct RotateInfo {
    int *start;
    int *end;
    int pin1;
    int pin2;
};

// VARIABLE
namespace ControlState {
    bool is_gear_up      = false;
    bool is_gear_down    = false;
    bool is_zero_force   = false;
    bool is_reverse      = false;
    bool invert          = false; // reverse
    bool fast_stop       = false;
    bool is_rotate_left  = false;
    bool is_rotate_right = false;
}

int CURRENT_GEAR = 0;
int robot_status = 0;
int max_different_rotate = 0;
int self_rotate_gap = 0;
int current_power_left = 0, current_power_right = 0;
int new_power_left = 0, new_power_right = 0;

int power_lift = 0;

std::vector<int> MIN_VALUE = {128, 256, 512, 1024};

float console_x_axis = 0, console_y_axis = 0;
float x_axis = 0, y_axis = 0;
int left_power = 0, right_power = 0;

PIN left_pin, right_pin;

std::vector<std::vector<int>> POWER_LEVEL(2, std::vector<int>(MAX_GEAR * 2 + 2, 0));    // lưu mức năng lượng
std::vector<std::vector<PIN>> MOTOR_PIN(2, std::vector<PIN>(MAX_GEAR * 2 + 2, {0, 0})); // lưu chân pin

#endif
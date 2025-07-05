#ifndef CONFIG_BUTTON_H
#define CONFIG_BUTTON_H

#include <array>
#include <vector>
#include <SimpleKalmanFilter.h>
#include "PCA9685.h"
#include "Arduino.h"


PCA9685 pwmController(B000000);  
uint16_t pwms[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// CONSOLE

#define GEAR_UP PSB_PAD_UP
#define GEAR_DOWN PSB_PAD_DOWN
#define ZERO_FORCE PSB_L3
#define REVERSE PSB_R3 

// MOTOR SETTING

int MIN_POWER = 0;
#define MAX_POWER 2560
#define MAX_GEAR 5
int LEFT_MESURE = 250;

// SERVO SETTING

#define MIN_ROTATE 80
#define MAX_ROTATE 570

#define DEFAULT_ANGLE 170
#define MIN_ANGLE_DEFAULT 0
#define MAX_ANGLE_DEFAULT 180
#define COLLECTOR_PIN 3
#define COLLECTOR_ROTATION_PIN 2

#define POWER_OPEN_COLLECTOR 100
#define POWER_CLOSE_COLLECTOR 500
#define POWER_HOLD_COLLECTOR 347
#define TIME_SET_HOLD_FRUIT 600
#define TIME_SET_COLLECTOR_CHANGE_SPEED 600

#define BASKET_CONTROL_PIN 4
#define BASKET_DEFAULT_ROTATION  120
#define BASKET_HIGH_DEFAULT_ROTATION 50

#define SUPPORT_ARM_SERVO_PIN 6
#define SUPPORT_ARM_SERVO_ROTATION 90


#define ARLAM_HIGH_BASKET_PIN 5

SimpleKalmanFilter motor_left_smooth(100, 0.1, 0.5);
SimpleKalmanFilter motor_right_smooth(100, 0.1, 0.5);

SimpleKalmanFilter motor_A_smooth(100, 1, 1);
SimpleKalmanFilter motor_B_smooth(100, 1, 1);

/*/////////////////////////////////////////////////////

All below here are temp variable for processing

*/
/////////////////////////////////////////////////////

// STRUCT
struct PIN {
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

    bool is_motor_a      = false;
    bool is_motor_b      = false;
    bool is_motor_a_reverse = false;
    bool is_motor_b_reverse = false;

    bool is_motor_left   = false;
    bool is_motor_right  = false;
    bool is_motor_left_reverse = false;
    bool is_motor_right_reverse = false;

    bool is_basket_hold = false;
}

int CURRENT_GEAR = 0;
int motor_power_A = 0, motor_power_B = 0;

float console_x_axis = 0, console_y_axis = 0;
float x_axis = 0, y_axis = 0;
int left_power = 0, right_power = 0;

std::vector<int> POWER_LEVEL(MAX_GEAR, 0);    // lưu mức năng lượng
std::array<int, 4> TURN_RATIO = { 0 , 50, 60, 70 };

#endif
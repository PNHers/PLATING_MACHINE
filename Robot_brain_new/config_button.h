#ifndef CONFIG_BUTTON_H
#define CONFIG_BUTTON_H

#include <vector>

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

#define MIN_POWER 0
#define MAX_POWER 4096
#define MAX_GEAR 6
#define MAX_ROTATE_SPEED 3 // giá trị này phải bé hơn MAX_GEAR 
#define when_to_rotate 0.9 // khi nào robot nên xoay
#define CHANGE_PULL 1
#define BRAKE 3

#define LEFT 0
#define RIGHT 1

#define LEFT_PIN_1 8 // đầu dương
#define LEFT_PIN_2 9
#define RIGHT_PIN_1 10 // đầu dương
#define RIGHT_PIN_2 11

// STRUCT
struct PIN{
  int pin1;
  int pin2;
};

// VARIABLE
bool gear_up = false;
bool gear_down = false;
bool zero_force = false;
bool is_reverse = false;
bool invert = false; // reverse

int CURRENT_GEAR = 0;
int robot_status = 0;
int current_power_left = 0, current_power_right = 0;
int new_power_left = 0, new_power_right = 0;

int power_lift = 0;

float x_axis = 0, y_axis = 0;


std::vector<std::vector<int>> POWER_LEVEL(2, std::vector<int>(MAX_GEAR * 2 + 2, 0)); //lưu mức năng lượng
std::vector<std::vector<PIN>> MOTOR_PIN(2, std::vector<PIN>(MAX_GEAR * 2 + 2, {0, 0})); //lưu chân pin


#endif
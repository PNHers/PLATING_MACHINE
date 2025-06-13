#include "time_control.h"
#include "RotateMotor.h"
#include "ps2_controler.h"
#include "config_button.h"

#define motorA1 8 // đầu dương
#define motorA2 9
#define motorB1 10 // đầu dương
#define motorB2 11

// trạng thái

#define IDLE 0.1 // stand
#define ALCT 0.95 // pull

#define time_to_pull 1

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int TIME = 0, NEW_TIME = 0;

void check_status(float y_axis){
  if(invert) y_axis *= -1;
  if(abs(y_axis) < IDLE){
    robot_status = HOLD;
  }
  else if (y_axis > 0) robot_status = PULL;
  else if (y_axis < 0) robot_status = BACK;
}


void setup() {
  pwm.begin();
  pwm.setOscillatorFrequency(27000000); 
  pwm.setPWMFreq(50);
  Wire.setClock(400000); 

  Serial.begin(115200);
  delay(100);
  Serial.println("Waiting for setup.....");
  delay(2000);
  
  setupPS2();
  Div_level();

  Serial.println("Setup done!");
  setup_timer();
}

void loop() {
  new_power_left = current_power_left, new_power_right = current_power_right;
  // tick_timer();
  ps2x.read_gamepad();
  CONSOL_READ();
  position_of_console(&x_axis, &y_axis);
  check_status(y_axis);
  // if(robot_status != 2) pull = false;
  // // Serial.println(robot_status);
  // if(robot_status == 2 && !pull){
  //   if(a_seconds(1)) pull = true;
  // }
  // int TIMES = get_time();
  // // Serial.println(TIMES);
  // move(x_axis, y_axis, robot_status, &invert, pull, &TIMES);
  // //rotate_2_motor(RotateInfo(&oldLeft, &left_motor, 8, 9), RotateInfo(&oldRight, &right_motor, 10, 11), &pwm);
  // // oldLeft = left_motor; oldRight = right_motor;
  // // delay(50);
  max_different_rotate = POWER_LEVEL[LEFT][MAX_GEAR + CURRENT_GEAR] * (TURN_RATIO / 100.0); 
  
  move2();
  RotateInfo left_motor = {&current_power_left, &new_power_left, left_pin.pin1, left_pin.pin2};
  RotateInfo right_motor = {&current_power_right, &new_power_right, right_pin.pin1, right_pin.pin2};

  rotate_2_motor(left_motor, right_motor, &pwm);

  current_power_left = new_power_left;
  current_power_right = new_power_right;

  // Serial.print(current_power_left);
  // Serial.print(" ");
  // Serial.println(current_power_right);

  unpress_button();

  // for(int i = 0; i < MAX_GEAR + MAX_GEAR; i++){
  //   Serial.println(POWER_LEVEL[LEFT][i]);
  // }
  delay(10);
}

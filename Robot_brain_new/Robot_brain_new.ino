#include "time_control.h"
#include "RotateMotor.h"
#include "ps2_controler.h"
#include "servo_controller.h"
#include "config_button.h"
#include "gyro_control.h"
#include <SimpleKalmanFilter.h>

// trạng thái

#define time_to_pull 1

// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// int BASE_TIME = 0 , TIME = 0, NEW_TIME = 0, DEM = 0;

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

  gyro_setup();
  // setupPS2();
  // servo_setup();
  // Div_level();

  setup_timer();
  // // gyro_setup_tick();
  // gyro_tick = 1.0 / myIMU.getGyroDataRate();
  // Serial.println(myIMU.getGyroDataRate());
  Serial.println("Setup done!");

}

void loop() {

  // // // tick_timer();
  // ps2x.read_gamepad();
  // CONSOL_READ();
  // position_of_console(&console_x_axis, &console_y_axis);
  // check_status(console_y_axis);

  // smooth_joystick();

  // // // if(robot_status != 2) pull = false;
  // // // // Serial.println(robot_status);
  // // // if(robot_status == 2 && !pull){
  // // //   if(a_seconds(1)) pull = true;
  // // // }
  // // // int TIMES = get_time();
  // // // // Serial.println(TIMES);
  // // // move(x_axis, y_axis, robot_status, &invert, pull, &TIMES);
  // // // //rotate_2_motor(RotateInfo(&oldLeft, &left_motor, 8, 9), RotateInfo(&oldRight, &right_motor, 10, 11), &pwm);
  // // // // oldLeft = left_motor; oldRight = right_motor;
  // // // // delay(50);
  // max_different_rotate = POWER_LEVEL[LEFT][MAX_GEAR + CURRENT_GEAR] * (TURN_RATIO / 100.0); 
  
  // if(CURRENT_GEAR == 0) self_rotate();
  // else if (!fast_stop && !rotate_left && !rotate_right) move2();

  
  // // RotateInfo left_motor = {&current_power_left, &new_power_left, left_pin.pin1, left_pin.pin2};
  // // RotateInfo right_motor = {&current_power_right, &new_power_right, right_pin.pin1, right_pin.pin2};

  // // rotate_2_motor(left_motor, right_motor, &pwm);
  // Serial.print(console_y_axis);
  // Serial.print(",");
  // Serial.print(x_axis);
  // Serial.print(",");

  // smooth_motor(&new_power_left, &new_power_right);

  // current_power_left = new_power_left;
  // current_power_right = new_power_right;

  // // // Serial.print(current_power_left);
  // // // Serial.print(" ");
  // // // Serial.println(current_power_right);
  // if(fast_stop) fast_stop = false;
  // unpress_button();
  
  get_accel();

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

  if(detect_movement()) Serial.println("Object is moving!");

  delay(5);
  
}

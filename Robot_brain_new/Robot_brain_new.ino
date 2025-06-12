#include "time_control.h"
#include "RotateMotor.h"
#include "ps2_controler.h"

#define motorA1 8 // đầu dương
#define motorA2 9
#define motorB1 10 // đầu dương
#define motorB2 11

// trạng thái
#define STAND 0
#define HOLD 1
#define PULL 2
#define BACK -1

#define IDLE 0.1 // stand
#define ALCT 0.95 // pull

#define time_to_pull 1

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
float x_axis = 0, y_axis = 0;
float oldLeft = 0, oldRight = 0;
int robot_status = 0, old_STATUS = 0;
bool invert = false, pull = false;
int TIME = 0, NEW_TIME = 0;

void check_status(float y_axis, bool invert){
  if(invert) y_axis *= -1;
  if(y_axis < 0) robot_status = BACK;
  else{
    if (y_axis <= IDLE) robot_status = STAND;
    else if(y_axis >= ALCT && robot_status != 2){
       robot_status = PULL;
       TIME = TIME_SECS;
    }
    else if(y_axis > IDLE && y_axis < ALCT) robot_status = HOLD;
  }
  if(TIME){
    NEW_TIME = TIME_SECS;
    if(NEW_TIME - TIME == time_to_pull){
      TIME = 0;
      pull = true;
    }
  } else pull = false;
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
  tick_timer();
  position_of_console(&x_axis, &y_axis);
  if(robot_status != 2) pull = false;
  check_status(y_axis, invert);
  Serial.println(robot_status);
  if(robot_status == 2 && !pull){
    if(a_seconds(1)) pull = true;
  }
  move(x_axis, y_axis, robot_status, &invert, pull);
  //rotate_2_motor(RotateInfo(&oldLeft, &left_motor, 8, 9), RotateInfo(&oldRight, &right_motor, 10, 11), &pwm);
  // oldLeft = left_motor; oldRight = right_motor;
  // delay(50);

}

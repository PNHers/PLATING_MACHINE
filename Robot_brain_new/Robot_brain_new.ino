#include "RotateMotor.h"
#include "ps2_controler.h"
#include "time_control.h"

#define motorA1 8 // đầu dương
#define motorA2 9
#define motorB1 10 // đầu dương
#define motorB2 11

// trạng thái
#define STAND 0
#define HOLD 1
#define PULL 2
#define BACK -1

#define IDLE 0.15 // stand
#define ALCT 0.9 // pull

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
float x_axis = 0, y_axis = 0;

float oldLeft = 0, oldRight = 0;

int robot_status = 0, old_STATUS = 0;

bool invert = false;

void check_status(float y_axis, bool invert){
  if(invert) y_axis *= -1;
  if(y_axis < 0) robot_status = BACK;
  else{
    if (y_axis <= IDLE) robot_status = STAND;
    else if(y_axis >= ALCT) robot_status = PULL;
    else robot_status = HOLD;
  }
}

void setup() {
  pwm.begin();
  pwm.setOscillatorFrequency(27000000); 
  pwm.setPWMFreq(50);
  Wire.setClock(400000); 


  Serial.begin(115200);
  delay(100);
  Serial.println("Starting.....");
  delay(2000);
  
  setup_timer();
  setupPS2();
}

void loop() {
  // put your main code here, to run repeatedly:
  position_of_console(&x_axis, &y_axis);
  //if (left_motor > 1 || left_motor < -1 || right_motor > 1 || right_motor < -1){Serial.println("ERROR HERE");}
  check_status(y_axis, invert);
  // Serial.println(robot_status);
  // move(x_axis, y_axis, STATUS, &invert);
  //rotate_2_motor(RotateInfo(&oldLeft, &left_motor, 8, 9), RotateInfo(&oldRight, &right_motor, 10, 11), &pwm);
  // oldLeft = left_motor; oldRight = right_motor;
  //delay(1000);
  Serial.println(rtc.getSeconds());
}

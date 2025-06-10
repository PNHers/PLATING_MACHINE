#include "RotateMotor.h"
#include "ps2_controler.h"

#define motorA1 8 // đầu dương
#define motorA2 9
#define motorB1 10 // đầu dương
#define motorB2 11

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
float left_motor = 0, right_motor = 0;

float oldLeft = 0, oldRight = 0;

void setup() {
  pwm.begin();
  pwm.setOscillatorFrequency(27000000); 
  pwm.setPWMFreq(50);
  Wire.setClock(400000); 


  Serial.begin(115200);
  delay(100);
  Serial.println("Starting.....");
  delay(2000);

  setupPS2();
}

void loop() {
  // put your main code here, to run repeatedly:
  TakeMotorRotate(&left_motor, &right_motor);
  //if (left_motor > 1 || left_motor < -1 || right_motor > 1 || right_motor < -1){Serial.println("ERROR HERE");}
  //rotate_2_motor(RotateInfo(&oldLeft, &left_motor, 8, 9), RotateInfo(&oldRight, &right_motor, 10, 11), &pwm);
  oldLeft = left_motor; oldRight = right_motor;
  //delay(1000);
}

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


  Serial.begin(9600);
  delay(100);
  Serial.println("Starting.....");
  delay(2000);
  safe_rotate(0.5, 1, 8, 9, &pwm);

  setupPS2();
}

void loop() {
  // put your main code here, to run repeatedly:
  TakeMotorRotate(&left_motor, &right_motor);
  //if (left_motor > 1 || left_motor < -1 || right_motor > 1 || right_motor < -1){Serial.println("ERROR HERE");}
  safe_rotate(oldLeft, left_motor, motorA1, motorA2, &pwm);
  safe_rotate(oldRight, right_motor, motorB1, motorB2, &pwm);
  
  delay(1000);
}

#ifndef ROTATEMOTOR_H
#define ROTATEMOTOR_H

#include <stdio.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <vector>

#define MENSURE_VAULE 0.05 // sai số khi joystick để ở vị trí ban đầu
#define max_power 4096
#define STEP 100

struct Rotate{
  int power;
  int pin1;
  int pin2;
};

struct RotateInfo{
  float start;
  float end;
  int pin1;
  int pin2;
};

void swap(int& a, int& b){
  int temp = a;
  a = b;
  b = temp;
}
// anh dung dep trai
void setPWMMotors(Rotate motor_info, Adafruit_PWMServoDriver* pwm){
  pwm.setPin(pin1, value);
  pwm.setPin(pin2, 0);

  Serial.print(motor_info.power);
  Serial.print(" pin: ");
  Serial.print(motor_info.pin1);
  Serial.print(" ");
  Serial.println(motor_info.pin2);
}

void smooth_increase_decrease(int start, int end, int step, int pin1, int pin2, std::vector<Rotate>* list_rotate){  
  if(start < end){
    for(int temp = start; temp <= end; temp += step){
        list_rotate->insert(list_rotate->end(), Rotate(temp, pin1, pin2));
    }
    list_rotate->insert(list_rotate->end(), Rotate(end, pin1, pin2));
  }
  else if (start > end){
    for(int temp = start; temp >= end; temp -= step){
        list_rotate->insert(list_rotate->end(), Rotate(temp, pin1, pin2));
    }
    list_rotate->insert(list_rotate->end(), Rotate(end, pin1, pin2));
  }
}

/*
  Hàm safe_rotate này chỉ điều khiển được một motor
  đối với trường hợp motor tiến (giá trị power dương) thì pin1 là chân cấp điện, pin2 là chân không có điện
  và ngược lại
*/

int safe_rotate(float power_current, float power_new, int pin1, int pin2, std::vector<Rotate>* list_rotate){ // power đi từ 1 đến -1. 0 là điểm trung gian

  bool isInvert = false;
  int value = abs(int(power_current * max_power));
  int value_new = abs(int(power_new * max_power));

  if(power_current * power_new < 0) isInvert = true;

  if(power_new <= MENSURE_VAULE && power_new >= -MENSURE_VAULE){
    // sức mạnh của motor bằng 0 khi giá trị power_new nằm trong vùng sai số;
    if(power_current > 0){
      smooth_increase_decrease(value, 0, STEP, pin1, pin2, list_rotate);
    }
    else if(power_current < 0){
      smooth_increase_decrease(value, 0, STEP, pin2, pin1, list_rotate);
    }
    return 1;
  }
  if(isInvert){
    if(power_current > 0){
      smooth_increase_decrease(value, 0, STEP, pin1, pin2, list_rotate);
      smooth_increase_decrease(0, value_new, STEP, pin2, pin1, list_rotate);
    }
    else if (power_current < 0){
      smooth_increase_decrease(value, 0, STEP, pin2, pin1, list_rotate);
      smooth_increase_decrease(0, value_new, STEP, pin1, pin2, list_rotate);
    }
  }
  else{
    if(power_current > 0){
      smooth_increase_decrease(value, value_new, STEP, pin1, pin2, list_rotate);
    }
    else if(power_current < 0){
      smooth_increase_decrease(value, value_new, STEP, pin2, pin1, list_rotate);
    }
    else{
      if(power_new > 0) smooth_increase_decrease(value, value_new, STEP, pin1, pin2, list_rotate);
      else if (power_new < 0) smooth_increase_decrease(value, value_new, STEP, pin2, pin1, list_rotate);
    }
  }
  return 0;
}

bool Check_number(Rotate new_rotate, Rotate present_rotate){
  if (new_rotate.pin1 != present_rotate.pin1 && new_rotate.power * present_rotate.power != 0){return false;}
  return true;
}

void smooth_rotate(std::vector<Rotate> RotateMotorbigger,std::vector<Rotate> RotateMotorsmaller, Adafruit_PWMServoDriver* pwm){
  int jump_nb = RotateMotorbigger.size();
  if (RotateMotorsmaller.size() > 0) { jump_nb = int(RotateMotorbigger.size() / RotateMotorsmaller.size());}

  int cout = 0;
  int BigMotorCout = 0;
  int SmallMotorCout = 0;
  // Serial.print("Bigger size:");
  // Serial.println(RotateMotorbigger.size());
  // Serial.print("Smaller size:");
  // Serial.println(RotateMotorsmaller.size());
  for (int k = 0; k < (RotateMotorbigger.size() + RotateMotorsmaller.size()); k++){
    if (cout == jump_nb && SmallMotorCout < RotateMotorsmaller.size()){
      if (SmallMotorCout == 0 || (Check_number(RotateMotorsmaller[SmallMotorCout], RotateMotorsmaller[SmallMotorCout - 1]))){
        setPWMMotors(RotateMotorsmaller[SmallMotorCout], pwm);}
      // Serial.print("Small: ");
      // Serial.println(SmallMotorCout);
      SmallMotorCout ++;
      cout = 0;
    }
    else {
      if (SmallMotorCout == 0 || (Check_number(RotateMotorsmaller[SmallMotorCout], RotateMotorsmaller[SmallMotorCout - 1]))){
        setPWMMotors(RotateMotorbigger[BigMotorCout], pwm);
      }// Serial.print("Big: ");
      // Serial.println(BigMotorCout);
      BigMotorCout ++;
      cout ++;
    } 
  }
}

void rotate_2_motor(RotateInfo motor1, RotateInfo motor2, Adafruit_PWMServoDriver* pwm){
  std::vector<Rotate> RotateMotor1, RotateMotor2;
  safe_rotate(motor1.start, motor1.end, motor1.pin1, motor1.pin2, &RotateMotor1);
  safe_rotate(motor2.start, motor2.end, motor2.pin1, motor2.pin2, &RotateMotor2);
  for (auto motor : RotateMotor1){
    if (motor.power > 4096){Serial.println("error");}
  }
  for (auto motor : RotateMotor2){
    if (motor.power > 4096){Serial.println("error");}
  }

  if (RotateMotor1.size() > RotateMotor2.size()) {smooth_rotate(RotateMotor1, RotateMotor2, pwm);return;}
  smooth_rotate(RotateMotor2, RotateMotor1, pwm);
  
  
  // Serial.println(RotateMotor1.size());
  // for (auto motor : RotateMotor1){
  //   Serial.print(motor.power);
  //   Serial.print(" pin: ");
  //   Serial.print(motor.pin1);
  //   Serial.print(" ");
  //   Serial.println(motor.pin2);
  // }

}


#endif
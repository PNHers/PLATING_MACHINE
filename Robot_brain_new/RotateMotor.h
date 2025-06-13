#ifndef ROTATEMOTOR_H
#define ROTATEMOTOR_H

#include <stdio.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "config_button.h"
#include <vector>

#define MENSURE_VAULE 0.05 // sai số khi joystick để ở vị trí ban đầu
#define max_power 2025
#define STEP 100

int MAX_LEVEL = MAX_GEAR + MAX_GEAR;
bool already_pull = false, is_rotate = false;

struct Rotate{
  int power;
  int pin1;
  int pin2;
};


void swap(int& a, int& b){ // ý là cái hàm này không xài nhưng vẫn giữ lại đây =))  
  int temp = a;
  a = b;
  b = temp;
}

void Div_level(){ // lưu các giá trị vào ma trận
  int temp = (MAX_POWER - MIN_POWER) / MAX_GEAR;
  for(int i = 1; i <= MAX_GEAR; i++){
    POWER_LEVEL[LEFT][i + MAX_GEAR] = POWER_LEVEL[LEFT][i + MAX_GEAR - 1] + temp;
    POWER_LEVEL[RIGHT][i + MAX_GEAR] = POWER_LEVEL[RIGHT][i + MAX_GEAR - 1] + temp;
    MOTOR_PIN[LEFT][i + MAX_GEAR] = {LEFT_PIN_1, LEFT_PIN_2};
    MOTOR_PIN[RIGHT][i + MAX_GEAR] = {RIGHT_PIN_1, RIGHT_PIN_2};
  }
  int k = MAX_GEAR + MAX_GEAR;
  for(int i = 0; i < MAX_GEAR; i++){
    POWER_LEVEL[LEFT][i] = POWER_LEVEL[LEFT][k];
    POWER_LEVEL[RIGHT][i] = POWER_LEVEL[RIGHT][k];
    k--;
    MOTOR_PIN[LEFT][i] = {LEFT_PIN_2, LEFT_PIN_1};
    MOTOR_PIN[RIGHT][i] = {RIGHT_PIN_2, RIGHT_PIN_1};
  }
  POWER_LEVEL[LEFT][0] = POWER_LEVEL[RIGHT][0] = MAX_POWER;
  POWER_LEVEL[LEFT][MAX_GEAR + MAX_GEAR] = POWER_LEVEL[RIGHT][MAX_GEAR + MAX_GEAR] = MAX_POWER;
  power_lift = temp / 2;
  self_rotate_gap = (MAX_POWER - MIN_POWER) * (SELF_ROTATE_RATIO / 100.0);
}

// anh dung dep trai
void setPWMMotors(Rotate motor_info, Adafruit_PWMServoDriver* pwm){
  if (motor_info.power < 0){return ;}
  pwm->setPin(motor_info.pin1, motor_info.power);
  pwm->setPin(motor_info.pin2, 0);
  delay(50);
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

void safe_rotate(int *power_current, int *power_new, int pin1, int pin2, std::vector<Rotate>* list_rotate){ // power đi từ 1 đến -1. 0 là điểm trung gian

  bool isInvert = false;
  // int value = abs(int(*power_current * max_power));
  // int value_new = abs(int(*power_new * max_power));
  int value = *power_current;
  int value_new = *power_new;

  if((*power_current) * (*power_new) < 0) isInvert = true;

  if(*power_new <= MENSURE_VAULE && *power_new >= -MENSURE_VAULE){
    // sức mạnh của motor bằng 0 khi giá trị *power_new nằm trong vùng sai số;
    if(*power_current > 0){
      smooth_increase_decrease(value, 0, STEP, pin1, pin2, list_rotate);
    }
    else if(*power_current < 0){
      smooth_increase_decrease(value, 0, STEP, pin2, pin1, list_rotate);
    }
    *power_new = 0;
  }// nothing
  else if(isInvert){
    if(*power_current > 0){
      smooth_increase_decrease(value, 0, STEP, pin1, pin2, list_rotate);
      smooth_increase_decrease(0, value_new, STEP, pin2, pin1, list_rotate);
    }
    else if (*power_current < 0){
      smooth_increase_decrease(value, 0, STEP, pin2, pin1, list_rotate);
      smooth_increase_decrease(0, value_new, STEP, pin1, pin2, list_rotate);
    }
  }
  else{
    if(*power_current > 0){
      smooth_increase_decrease(value, value_new, STEP, pin1, pin2, list_rotate);
    }
    else if(*power_current < 0){
      smooth_increase_decrease(value, value_new, STEP, pin2, pin1, list_rotate);
    }
    else{
      if(*power_new > 0) smooth_increase_decrease(value, value_new, STEP, pin1, pin2, list_rotate);
      else if (*power_new < 0) smooth_increase_decrease(value, value_new, STEP, pin2, pin1, list_rotate);
    }
  }
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


void FAST_MOTOR_STOP(){ // làm motor dừng đột ngột
  if(!invert){    
    left_pin = MOTOR_PIN[LEFT][MAX_GEAR + 1];
    right_pin = MOTOR_PIN[RIGHT][MAX_GEAR + 1];
  }
  else{
    left_pin = MOTOR_PIN[LEFT][MAX_GEAR - 1];
    right_pin = MOTOR_PIN[RIGHT][MAX_GEAR - 1];
  }
  while(new_power_left != 0 || new_power_right != 0){
    if(new_power_left > 0) new_power_left -= power_lift;
    if(new_power_right > 0) new_power_right -= power_lift;
    if(new_power_left < 0) new_power_left = 0;
    if(new_power_right < 0) new_power_right = 0;
  }
}

void OVER_GEAR(){ // luôn đi đúng tốc độ
  if(new_power_left > POWER_LEVEL[LEFT][MAX_GEAR + CURRENT_GEAR] || new_power_right > POWER_LEVEL[RIGHT][MAX_GEAR + CURRENT_GEAR]){
    new_power_left -= power_lift;
    new_power_right -= power_lift; 
    if(new_power_left < POWER_LEVEL[LEFT][MAX_GEAR + CURRENT_GEAR]) new_power_left = POWER_LEVEL[LEFT][MAX_GEAR + CURRENT_GEAR];
    if(new_power_right < POWER_LEVEL[RIGHT][MAX_GEAR + CURRENT_GEAR]) new_power_right = POWER_LEVEL[RIGHT][MAX_GEAR + CURRENT_GEAR];
    delay(100);
  }
}

void move2(){
  //  Serial.println(CURRENT_GEAR);
   if(invert && CURRENT_GEAR > 0) CURRENT_GEAR *= -1;
   left_pin = MOTOR_PIN[LEFT][MAX_GEAR + CURRENT_GEAR];
   right_pin = MOTOR_PIN[RIGHT][MAX_GEAR + CURRENT_GEAR];
   if(robot_status == 1){
      new_power_left += power_lift * y_axis;
      new_power_right += power_lift * y_axis;
      if(new_power_left > POWER_LEVEL[LEFT][MAX_GEAR + CURRENT_GEAR]) new_power_left = POWER_LEVEL[LEFT][MAX_GEAR + CURRENT_GEAR];
      if(new_power_right > POWER_LEVEL[RIGHT][MAX_GEAR + CURRENT_GEAR]) new_power_right = POWER_LEVEL[RIGHT][MAX_GEAR + CURRENT_GEAR];
      delay(100);
   }
   if(robot_status == -1){
      new_power_left += power_lift * y_axis;
      new_power_right += power_lift * y_axis;
      if(new_power_left < 0) new_power_left = 0;
      if(new_power_right < 0) new_power_right = 0;
      delay(50);
   }
   OVER_GEAR();
   if(abs(x_axis) > when_to_rotate ){
      if(x_axis > 0) {
        new_power_right -= power_lift * x_axis;
        if (new_power_left - new_power_right > max_different_rotate) new_power_right = new_power_left - max_different_rotate;
        if (new_power_right < 0) new_power_right = 0;
      }
      if(x_axis < 0){
        new_power_left -= power_lift * -x_axis;
        if(new_power_right - new_power_left > max_different_rotate) new_power_left = new_power_right - max_different_rotate;
        if (new_power_left < 0) new_power_left = 0;
      }
      delay(50);
   }
   else{
      if (new_power_left > new_power_right){
        new_power_right += power_lift;
        if(new_power_right > new_power_left) new_power_right = new_power_left;
        delay(50);
      }
      else if (new_power_left < new_power_right){
        new_power_left += power_lift;
        if(new_power_right < new_power_left) new_power_left = new_power_right;
        delay(50);
      }
   }
   if(invert && CURRENT_GEAR < 0) CURRENT_GEAR *= -1;
}

void self_rotate(){
  OVER_GEAR();
  left_pin = MOTOR_PIN[LEFT][MAX_GEAR + 1];
  right_pin = MOTOR_PIN[RIGHT][MAX_GEAR + 1];
  if(abs(x_axis) > when_to_rotate ){
      if(x_axis > 0 && !rotate_left) {
        new_power_left += (self_rotate_gap * 0.5) * x_axis;
        new_power_right += (self_rotate_gap * 0.5) * x_axis;
        if(new_power_left > self_rotate_gap) new_power_left = self_rotate_gap;
        if(new_power_right > self_rotate_gap) new_power_right = self_rotate_gap;
        right_pin = MOTOR_PIN[RIGHT][MAX_GEAR - 1];
        rotate_right = true;
      }
      if(x_axis < 0 && !rotate_right){
        new_power_right += (self_rotate_gap * 0.5) * -x_axis;
        new_power_left += (self_rotate_gap * 0.5) * -x_axis;
        if(new_power_right > self_rotate_gap) new_power_right = self_rotate_gap;
        if(new_power_left > self_rotate_gap) new_power_left = self_rotate_gap;
        left_pin = MOTOR_PIN[LEFT][MAX_GEAR - 1];
        rotate_left = true;
      }
      if (x_axis > 0 && rotate_left){
        new_power_right -= self_rotate_gap * 0.5;
        new_power_left -= self_rotate_gap * 0.5;
        if(new_power_right < 0) new_power_right = 0;
        if(new_power_left < 0) new_power_left = 0;
        left_pin = MOTOR_PIN[LEFT][MAX_GEAR - 1]; 
      }
      if (x_axis < 0 && rotate_right){
        new_power_left -= self_rotate_gap * 0.5;
        new_power_right -= self_rotate_gap * 0.5;
        if(new_power_left < 0) new_power_left = 0;
        if(new_power_right < 0) new_power_right = 0;
        right_pin = MOTOR_PIN[RIGHT][MAX_GEAR - 1];
      }
      delay(50);
   }
   else{
      if(rotate_right){
        new_power_left -= self_rotate_gap * 0.5;
        new_power_right -= self_rotate_gap * 0.5;
        if(new_power_left < 0) new_power_left = 0;
        if(new_power_right < 0) new_power_right = 0;
        right_pin = MOTOR_PIN[RIGHT][MAX_GEAR - 1];
      }
      else if (rotate_left){
        new_power_right -= self_rotate_gap * 0.5;
        new_power_left -= self_rotate_gap * 0.5;
        if(new_power_right < 0) new_power_right = 0;
        if(new_power_left < 0) new_power_left = 0;
        left_pin = MOTOR_PIN[LEFT][MAX_GEAR - 1];
      }
      delay(50);
   }
   if((rotate_left || rotate_right) && new_power_left == new_power_right && new_power_left == 0){
      rotate_left = false;
      rotate_right = false;
   }
}


#endif
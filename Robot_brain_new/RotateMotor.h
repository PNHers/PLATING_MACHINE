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

struct RotateInfo{
  float* start;
  float* end;
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
}

// anh dung dep trai
void setPWMMotors(Rotate motor_info, Adafruit_PWMServoDriver* pwm){
  if (motor_info.power < 0){return ;}
  pwm->setPin(motor_info.pin1, motor_info.power);
  pwm->setPin(motor_info.pin2, 0);
  // delay(100);
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

void safe_rotate(float *power_current, float *power_new, int pin1, int pin2, std::vector<Rotate>* list_rotate){ // power đi từ 1 đến -1. 0 là điểm trung gian

  bool isInvert = false;
  int value = abs(int(*power_current * max_power));
  int value_new = abs(int(*power_new * max_power));

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

// void move(float x_axis, float y_axis, int robot_status, bool* invert, bool isPull, int* TIME_SECS){
//   int new_power_left = current_power_left, new_power_right = current_power_right;
//   if(!isPull && already_pull) isPull = true;
//   if(abs(x_axis) >= when_to_rotate) is_rotate = true;
//   else is_rotate = false;
//   // NEW_MAX_GEAR = *TIME_SECS;

//   if(robot_status == 1){
//     if(*invert){
//       if(x_axis < 0){
//         if(current_power_right - current_power_left <= MAX_ROTATE_SPEED) new_power_right += 1;
//         if (new_power_right > MAX_GEAR) new_power_right = LEVEL;
//       }
//       else{
//         if(current_power_left - current_power_right <= MAX_ROTATE_SPEED) new_power_left += 1;
//         if (new_power_left > MAX_GEAR) new_power_left = LEVEL;
//       }
//     }
//     else{
//       if(x_axis < 0){
//         if(current_power_right - current_power_left <= MAX_ROTATE_SPEED) new_power_left -= 1;
//         if (new_power_left < MAX_GEAR) new_power_left = LEVEL;
//       }
//       else{
//         if(current_power_left - current_power_right <= MAX_ROTATE_SPEED) new_power_right -= 1;
//         if (new_power_right < MAX_GEAR) new_power_right = LEVEL;
//       }
//     }
//   }
//   if(robot_status == 2 && isPull){
//     if(already_pull){
//       if(NEW_MAX_GEAR - MAX_GEAR >= CHANGE_PULL){
//         if(*invert){
//           new_power_left -= 1;
//           new_power_right -= 1;
//           MAX_GEAR = *TIME_SECS;
//         }
//         else{
//           new_power_left += 1;
//           new_power_right += 1;
//           MAX_GEAR = *TIME_SECS;
//         }
        
//       }
//     }
//     else if (!already_pull){
//       if(*invert){
//         new_power_left -= 1;
//         new_power_right -= 1;
//       }
//       else{
//         new_power_left += 1;
//         new_power_right += 1;
//       }
//       already_pull = true;
//       MAX_GEAR = *TIME_SECS;
//     }
//   }
//   if(robot_status == 0 && current_power_left == current_power_right && current_power_left == MAX_GEAR && is_rotate){
//     if(*invert) *invert = false;
//     if (x_axis >= when_to_rotate){ // rotate right
//       new_power_left += MAX_ROTATE_SPEED;
//       new_power_right -= MAX_ROTATE_SPEED;
//     }
//     else if (x_axis <= -when_to_rotate){ // rotate left
//       new_power_left -= MAX_ROTATE_SPEED;
//       new_power_right += MAX_ROTATE_SPEED;
//     }
//     else{
//       // đứng yên
//       new_power_left = new_power_right = MAX_GEAR;
//     }
//   }
//   if(!robot_status && !is_rotate){
//     if(new_power_left > MAX_GEAR) new_power_left -= 1;
//     if(new_power_right > MAX_GEAR) new_power_right -= 1;
//     if(new_power_left < MAX_GEAR) new_power_left += 1;
//     if(new_power_right < MAX_GEAR) new_power_right += 1;
//     delay(100);
//   }
//   if (robot_status == -1){
//     if(current_power_left == current_power_right && current_power_left == MAX_GEAR){
//       bool temp = *invert;
//       if(temp) temp = false;
//       else temp = true;
//       *invert = temp;
//       delay(100);
//     }
//     else{
//       if(*invert){
//         new_power_left += BRAKE;
//         new_power_right += BRAKE;
//         if(new_power_left > LEVEL) new_power_left = LEVEL;
//         if(new_power_right > LEVEL) new_power_right = LEVEL;
//         delay(200);
//       }
//       else{
//         new_power_left -= BRAKE;
//         new_power_right -= BRAKE;
//         if(new_power_left < LEVEL) new_power_left = LEVEL;
//         if(new_power_right < LEVEL) new_power_right = LEVEL;
//         delay(200);
//       }
//     }
//   }

//   if(robot_status != 2 && already_pull){
//     already_pull = false;
//   }

//   if(new_power_left > MAX_LEVEL) new_power_left = MAX_LEVEL;
//   if(new_power_right > MAX_LEVEL) new_power_right = MAX_LEVEL;  
//   if(new_power_left < 0) new_power_left = 0;
//   if(new_power_right < 0) new_power_right = 0;  

//   current_power_left = new_power_left;
//   current_power_right = new_power_right;
//   Serial.print(new_power_left);
//   Serial.print(" : ");
//   Serial.println(new_power_right);

//   // Serial.print(MAX_GEAR);
//   // Serial.print(" ");
//   // Serial.println(NEW_MAX_GEAR);
// }

void move2(){
   int new_power_left = current_power_left, new_power_right = current_power_right;
   Serial.println(CURRENT_GEAR);
   if(robot_status == 1){
      new_power_left += power_lift * y_axis;
      new_power_right += power_lift * y_axis;
      if(new_power_left > POWER_LEVEL[LEFT][MAX_GEAR + CURRENT_GEAR]) new_power_left = POWER_LEVEL[LEFT][MAX_GEAR + CURRENT_GEAR];
      if(new_power_right > POWER_LEVEL[RIGHT][MAX_GEAR + CURRENT_GEAR]) new_power_right = POWER_LEVEL[RIGHT][MAX_GEAR + CURRENT_GEAR];
      delay(200);
   }
   if(robot_status == -1){
      new_power_left += power_lift * y_axis;
      new_power_right += power_lift * y_axis;
      if(new_power_left < 0) new_power_left = 0;
      if(new_power_right < 0) new_power_right = 0;
      delay(200);
   }
   Serial.print(new_power_left);
   Serial.print(" ");
   Serial.println(new_power_right);

   current_power_left = new_power_left;
   current_power_right = new_power_right;
}


#endif
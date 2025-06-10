#include "HardwareSerial.h"
#ifndef PS2_CONTROLER_H
#define PS2_CONTROLER_H

#include <Wire.h> //thư viện I2c của Arduino, do PCA9685 sử dụng chuẩn giao tiếp i2c nên thư viện này bắt buộc phải khai báo 
#include <PS2X_lib.h> // Khai báo thư viện

//Định nghĩa các chân điều khiển 
#define PS2_DAT 12 // MISO 
#define PS2_CMD 13 // MOSI 
#define PS2_SEL 15 // SS 
#define PS2_CLK 14 // SLK

const float sai_so = 0.01; // sai số

PS2X ps2x; // khởi tạo class PS2x

void check(float* x_ps2, float* y_ps2){ //hàm này loại trừ sai số của tay cầm
  if(*x_ps2 <= sai_so && *x_ps2 >= -sai_so) *x_ps2 = 0.0f;
  if(*y_ps2 <= sai_so && *y_ps2 >= -sai_so) *y_ps2 = 0.0f;
}

void rotate(float x_ps2, float y_ps2, float *left_rotate, float *right_rotate){
  check(&x_ps2, &y_ps2); // kiểm tra sai số
  float length = sqrt(x_ps2*x_ps2 + y_ps2*y_ps2);
  float left_motor = 1, right_motor = 1;
  float actually_x_axis = x_ps2, actually_y_axis = y_ps2;

  // check nếu 2 giá trị được nhập từ console lớn hơn 1 || -1 (có sai số khi test);
  if(x_ps2 > 1) actually_x_axis = 1;
  else if (x_ps2 < -1) actually_x_axis = -1;
  if(y_ps2 > 1) actually_y_axis = 1;
  else if (y_ps2 <-1) actually_y_axis = -1;

  float cos_a = x_ps2 / length;

  if(length <= 1.00){
      if (x_ps2 >= 0) {
        actually_x_axis = length * cos_a;
        actually_y_axis = sqrt(length * length - actually_x_axis * actually_x_axis);
      }
      else{
        actually_y_axis = -(length * cos_a);
        if(y_ps2 == 0) actually_y_axis = 0;
        actually_x_axis = -sqrt(length * length - actually_y_axis * actually_y_axis);
      }
  }
  else{
      if (x_ps2 >= 0 || abs(y_ps2) == 1) {
        actually_x_axis = 1.00 * cos_a;
        actually_y_axis = sqrt(1 - actually_x_axis * actually_x_axis);
      }
      else{
        actually_y_axis = -(1.00 * cos_a);
        actually_x_axis = -sqrt(1 - actually_y_axis * actually_y_axis);
      }
  }

  if(y_ps2 < 0) actually_y_axis *= -1; 

  // chỗ này không có thì kết quả cuối ra giá trị nan
  if(x_ps2 == 0) actually_x_axis = 0;
  if(y_ps2 == 0) actually_y_axis = 0;

  // Serial.print(actually_x_axis);
  // Serial.print(" ");
  // Serial.println(actually_y_axis);

  length = sqrt(x_ps2*x_ps2 + y_ps2*y_ps2);
  if(length > 1) length = 1;
  left_motor = length, right_motor = length;

  cos_a = actually_x_axis / length;

  if (y_ps2 < 0) {
    left_motor *= -1; right_motor *= -1;

    if (cos_a < 0){
      float right_move = (-cos_a - 0.5)*(-2);
      right_motor *= right_move;
    }
    else if (cos_a > 0) {
      float left_move = (cos_a - 0.5)*(-2);
      left_motor *= left_move;
    }
  }
  else{
    // adding somthign
    if (cos_a > 0){
      float right_move = (cos_a - 0.5)*(-2);
      right_motor *= right_move;
    }
    else if (cos_a < 0) {
      float left_move = (-cos_a - 0.5)*(-2);
      left_motor *= left_move;
    }
  }

  Serial.print(left_motor);
  Serial.print(" : ");
  Serial.println(right_motor);

  *left_rotate = left_motor;
  *right_rotate = right_motor;
} 

void setupPS2() {
  int error = -1; 
  for (int i = 0; i < 10; i++) // thử kết nối với tay cầm ps2 trong 10 lần 
  {
    delay(1000); // đợi 1 giây 
    // cài đặt chân và các chế độ: GamePad
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false); 
    Serial.print("."); 
    if(!error) //kiểm tra nếu tay cầm đã kết nối thành công 
    break; // thoát khỏi vòng lặp 
  } 
  
}

void TakeMotorRotate(float* leftMotor, float* rightMotor) {
  // put your main code here, to run repeatedly:
  ps2x.read_gamepad(); // gọi hàm để đọc tay điều khiển 

  // Serial.print(ps2x.Analog(PSS_LX));
  // Serial.print(" ");
  // Serial.println(ps2x.Analog(PSS_LY));
  

  float handled_psx = (ps2x.Analog(PSS_LX)  - 127.5) / 127.5; 
  float handled_psy = (ps2x.Analog(PSS_LY)  - 127.5) / 127.5; 

  // Serial.print(handled_psx);
  // Serial.print(" ");
  // Serial.println(handled_psy);

  rotate(handled_psx, -handled_psy, leftMotor, rightMotor);
}

#endif
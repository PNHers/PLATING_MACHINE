#include <Wire.h> //thư viện I2c của Arduino, do PCA9685 sử dụng chuẩn giao tiếp i2c nên thư viện này bắt buộc phải khai báo 

#include <PS2X_lib.h> // Khai báo thư viện

//Định nghĩa các chân điều khiển 
#define PS2_DAT 12 // MISO 
#define PS2_CMD 13 // MOSI 
#define PS2_SEL 15 // SS 
#define PS2_CLK 14 // SLK

PS2X ps2x; // khởi tạo class PS2x

void rotate(float x_ps2, float y_ps2, float *left_rotate, float *right_rotate){
  float length = sqrt(x_ps2*x_ps2 + y_ps2*y_ps2);
  float left_motor = length, right_motor = length;

  if (y_ps2 > 0) {left_motor *= -1; right_motor *= -1;}

  float cos_a = x_ps2 / length;

  if (cos_a > 0){
    float right_move = (cos_a - 0.5)*(-2);
    right_motor *= right_move;
  }
  else if (cos_a < 0) {
    float left_move = (-cos_a - 0.5)*(-2);
    left_motor *= left_move;
  }
  Serial.print(left_motor);
  Serial.print(" : ");
  Serial.println(right_motor);

  *left_rotate = left_motor;
  *right_rotate = right_motor;
} 

void setup() {
  // put your setup code here, to run once:
  //Khởi tạo Serial monitor với tốc độ 115200
  Serial.begin(115200);

  //Kết nối với tay cầm bằng hàm ps2x.config_gamepad, thử kết nối lại trong vòng 10 lần nếu quá 10 lần không kết nối được với tay cầm thì sẽ dừng lại

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

void loop() {
  // put your main code here, to run repeatedly:
  ps2x.read_gamepad(); // gọi hàm để đọc tay điều khiển 

  Serial.print("Stick Values:");
  Serial.print(ps2x.Analog(PSS_LX), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
  Serial.print(",");
  Serial.println(ps2x.Analog(PSS_LY), DEC);

  float handled_psx = (ps2x.Analog(PSS_LX)  - 127.5) / 127.5; 
  float handled_psy = (ps2x.Analog(PSS_LY)  - 127.5) / 127.5; 

  Serial.print("Handled Values:");
  Serial.print(handled_psx); //Left stick, Y axis. Other options: LX, RY, RX  
  Serial.print(",");
  Serial.println(handled_psy);
  
  float rightR = 0, leftR = 0;
  Serial.print("Motor run at :");
  rotate(handled_psx, handled_psy, &leftR, &rightR);

  delay(1000);

}

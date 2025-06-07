
#define MENSURE_VAULE 0.05 // sai số khi joystick để ở vị trí ban đầu
#define max_power 4096
#define STEP 100

void swap(int& a, int& b){
  int temp = a;
  a = b;
  b = temp;
}

void setPWMMotors(int value, int pin1, int pin2){
   //pwm.setPin(pin1, value);
   //pwm.setPin(pin2, 0);
}

void smooth_increase_decrease(int start, int end, int step, int pin1, int pin2){
  if(start < end){
    for(int temp = start; temp <= end; temp += step){
        setPWMMotors(temp, pin1, pin2);
    }
    setPWMMotors(end, pin1, pin2);
  }
  else if (start > end){
    for(int temp = start; temp <= end; temp -= step){
        setPWMMotors(temp, pin1, pin2);
    }
    setPWMMotors(end, pin1, pin2);
  }
}

/*
  Hàm safe_rotate này chỉ điều khiển được một motor
  đối với trường hợp motor tiến (giá trị power dương) thì pin1 là chân cấp điện, pin2 là chân không có điện
  và ngược lại
*/

void safe_rotate(float power_current, float power_new, int pin1, int pin2){ // power đi từ 1 đến -1. 0 là điểm trung gian

  bool isInvert = false;
  int value = abs(int(-power_current * max_power));
  int value_new = abs(int(power_new * max_power));

  if(power_current * power_new < 0) isInvert = true;

  if(power_new <= MENSURE_VAULE && power_new >= -MENSURE_VAULE){
    // sức mạnh của motor bằng 0 khi giá trị power_new nằm trong vùng sai số;
    if(power_current > 0){
      smooth_increase_decrease(value, 0, STEP, pin1, pin2);
    }
    else if(power_current < 0){
      smooth_increase_decrease(value, 0, STEP, pin2, pin1);
    }
    return;
  }
  if(isInvert){
    if(power_current > 0){
      smooth_increase_decrease(value, 0, STEP, pin1, pin2);
      smooth_increase_decrease(0, value_new, STEP, pin2, pin1);
    else if (power_current < 0){
      smooth_increase_decrease(value, 0, STEP, pin2, pin1);
      smooth_increase_decrease(0, value_new, STEP, pin1, pin2);
  }
  else{
    if(power_current > 0){
      smooth_increase_decrease(value, value_new, STEP, pin1, pin2);
    }
    else if(power_current < 0){
      smooth_increase_decrease(value, value_new, STEP, pin2, pin1);
    }
  }
}

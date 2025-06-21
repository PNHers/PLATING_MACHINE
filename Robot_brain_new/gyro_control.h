#ifndef GYRO_CONTROL_H
#define GYRO_CONTROL_H

#include "SparkFunLSM6DSO.h"
#include "Wire.h"
#include "config_button.h"

LSM6DSO myIMU; 

int data; 

void gyro_setup(){
  Wire.begin();
  delay(10);
  if( myIMU.begin() )
    Serial.println("Ready.");
  else { 
    Serial.println("Could not connect to IMU.");
    Serial.println("Freezing");
  }

  if( myIMU.initialize(BASIC_SETTINGS) ) Serial.println("Loaded Settings.");

  Serial.println("Please wait for gyro calibrate.....");
  for(int i = 0; i < samples; i++){
    A_OFFSET_X += myIMU.readFloatAccelX();
    A_OFFSET_Y += myIMU.readFloatAccelY();
    A_OFFSET_Z += myIMU.readFloatAccelZ();
    GYRO_OFFSET_X += myIMU.readFloatGyroX();
    GYRO_OFFSET_Y += myIMU.readFloatGyroY();
    GYRO_OFFSET_Z += myIMU.readFloatGyroZ();
    delay(2);
  }
  A_OFFSET_X /= samples;
  A_OFFSET_Y /= samples;
  A_OFFSET_Z /= samples;
  GYRO_OFFSET_X /= samples;
  GYRO_OFFSET_Y /= samples;
  GYRO_OFFSET_Z /= samples;
  Serial.println("Calibrate done!");
}

void three_variables_stab(float* a, float* b, float* c){
  *a = gyro_stab.updateEstimate(*a);
  *b = gyro_stab.updateEstimate(*b);
  *c = gyro_stab.updateEstimate(*c);
}

void get_accel(){
  //Get all parameters
  // Serial.print("\nAccelerometer:\n");
  // Serial.print(" X = ");
  // Serial.println(myIMU.readFloatAccelX(), 3);
  // Serial.print(" Y = ");
  // Serial.println(myIMU.readFloatAccelY(), 3);
  // Serial.print(" Z = ");
  // Serial.println(myIMU.readFloatAccelZ(), 3);

  // Serial.print("\nGyroscope:\n");
  // Serial.print(" X = ");
  // Serial.println(myIMU.readFloatGyroX(), 3);
  // Serial.print(" Y = ");
  // Serial.println(myIMU.readFloatGyroY(), 3);
  // Serial.print(" Z = ");
  // Serial.println(myIMU.readFloatGyroZ(), 3);

  // Serial.print("\nThermometer:\n");
  // Serial.print(" Degrees F = ");
  // Serial.println(myIMU.readTempF(), 3);
  
  // delay(200);

  A_X = myIMU.readFloatAccelX() - A_OFFSET_X;
  A_Y = myIMU.readFloatAccelY() - A_OFFSET_Y;
  A_Z = myIMU.readFloatAccelZ() - A_OFFSET_Z;

  GYRO_X = myIMU.readFloatGyroX() - GYRO_OFFSET_X;
  GYRO_Y = myIMU.readFloatGyroY() - GYRO_OFFSET_Y;
  GYRO_Z = myIMU.readFloatGyroZ() - GYRO_OFFSET_Z;

  three_variables_stab(&GYRO_X, &GYRO_Y, &GYRO_Z);
}

void get_range_gyro(){
  Serial.println(myIMU.getGyroRange());
}

#endif
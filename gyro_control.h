#ifndef GYRO_CONTROL_H
#define GYRO_CONTROL_H

#include "esp32-hal.h"
#include "SparkFunLSM6DSO.h"
#include "Wire.h"

const int SAMPLES = 2000;
const float MIN_ACCEL = 0.3;

/*
* Gyroscope axes:
* X: Roll
* Y: Pitch
* Z: Yaw
*/
namespace GyroSettings {
    SimpleKalmanFilter accel_x_filter(0.1, 0.1, 0.5);

    SimpleKalmanFilter gyro_x_filter(10, 6, 1);
    SimpleKalmanFilter gyro_y_filter(10, 6, 1);
    SimpleKalmanFilter gyro_z_filter(10, 6, 1);

    float accel_x = 0, accel_y = 0, accel_z = 0;
    float gyro_x = 0, gyro_y = 0, gyro_z = 0;
    
    float gyro_offset_x = 0, gyro_offset_y = 0, gyro_offset_z = 0;
    float accel_offset_x = 0, accel_offset_y = 0, accel_offset_z = 0;

    int oldVelocityTime = 0, newVelocityTime = 0;

    //cm/s
    float velocity = 0;
    
    // in degrees
    float roll = 0, pitch = 0, yaw = 0;
}

LSM6DSO myIMU;

void initGyro() {
    using namespace GyroSettings;

    Wire.begin();
    delay(10);

    if (myIMU.begin()) {
        Serial.println("Ready.");
    } else {
        Serial.println("Could not connect to IMU.");
        Serial.println("Freezing");
    }

    if (myIMU.initialize(BASIC_SETTINGS)) {
        Serial.println("Loaded Settings.");
    }

    myIMU.setHighPerfGyro(true);
    myIMU.setGyroDataRate(6660);
    myIMU.setGyroRange(2000);

    Serial.println("Calibrating gyroscope...");

    for (int i = 0; i < SAMPLES; i++){
        accel_offset_x += myIMU.readFloatAccelX();
        accel_offset_y += myIMU.readFloatAccelY();
        accel_offset_z += myIMU.readFloatAccelZ();
        gyro_offset_x += myIMU.readFloatGyroX();
        gyro_offset_y += myIMU.readFloatGyroY();
        gyro_offset_z += myIMU.readFloatGyroZ();
        delay(1);
    }

    accel_offset_x /= SAMPLES;
    accel_offset_y /= SAMPLES;
    accel_offset_z /= SAMPLES;
    gyro_offset_x /= SAMPLES;
    gyro_offset_y /= SAMPLES;
    gyro_offset_z /= SAMPLES;
    Serial.println("Calibration done!");
}

void get_accel() {
    using namespace GyroSettings;
    // Get all parameters
    //  Serial.print("\nAccelerometer:\n");
    //  Serial.print(" X = ");
    //  Serial.println(myIMU.readFloatAccelX(), 3);
    //  Serial.print(" Y = ");
    //  Serial.println(myIMU.readFloatAccelY(), 3);
    //  Serial.print(" Z = ");
    //  Serial.println(myIMU.readFloatAccelZ(), 3);

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

    accel_x = myIMU.readFloatAccelX() - accel_offset_x;
    accel_y = myIMU.readFloatAccelY() - accel_offset_y;
    accel_z = myIMU.readFloatAccelZ() - accel_offset_z;

    gyro_x = myIMU.readFloatGyroX() - gyro_offset_x;
    gyro_y = myIMU.readFloatGyroY() - gyro_offset_y;
    gyro_z = myIMU.readFloatGyroZ() - gyro_offset_z;

    accel_x = accel_x_filter.updateEstimate(accel_x);
}

bool isRobotMoving() {
    using namespace GyroSettings;
    return (abs(accel_x) > MIN_ACCEL || abs(accel_y) > MIN_ACCEL || abs(accel_z) > MIN_ACCEL);
}

void calculateVelocity(){
    GyroSettings::newVelocityTime = millis();
    GyroSettings::velocity += (GyroSettings::accel_x * 9.81f) / ((GyroSettings::newVelocityTime - GyroSettings::oldVelocityTime) * 1e-3);
    GyroSettings::oldVelocityTime = millis();
}

#endif
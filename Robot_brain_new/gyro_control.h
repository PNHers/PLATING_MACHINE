#ifndef GYRO_CONTROL_H
#define GYRO_CONTROL_H

#include "esp32-hal.h"
#include "SparkFunLSM6DSO.h"
#include "Wire.h"
#include "config_button.h"

LSM6DSO myIMU;

long data, OLD_TIME = 0, TIME = 0;

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

    for (int i = 0; i < samples; i++){
        A_OFFSET_X += myIMU.readFloatAccelX();
        A_OFFSET_Y += myIMU.readFloatAccelY();
        A_OFFSET_Z += myIMU.readFloatAccelZ();
        GYRO_OFFSET_X += myIMU.readFloatGyroX();
        GYRO_OFFSET_Y += myIMU.readFloatGyroY();
        GYRO_OFFSET_Z += myIMU.readFloatGyroZ();
        delay(1);
    }

    A_OFFSET_X /= samples;
    A_OFFSET_Y /= samples;
    A_OFFSET_Z /= samples;
    GYRO_OFFSET_X /= samples;
    GYRO_OFFSET_Y /= samples;
    GYRO_OFFSET_Z /= samples;
    Serial.println("Calibration done!");
    OLD_TIME = millis();
}

void three_variables_stab(float *a, float *b, float *c) {
    *a = gyro_x_stab.updateEstimate(*a);
    *b = gyro_y_stab.updateEstimate(*b);
    *c = gyro_z_stab.updateEstimate(*c);
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

    A_X = myIMU.readFloatAccelX() - A_OFFSET_X;
    A_Y = myIMU.readFloatAccelY() - A_OFFSET_Y;
    A_Z = myIMU.readFloatAccelZ() - A_OFFSET_Z;

    GYRO_X = myIMU.readFloatGyroX() - GYRO_OFFSET_X;
    GYRO_Y = myIMU.readFloatGyroY() - GYRO_OFFSET_Y;
    GYRO_Z = myIMU.readFloatGyroZ() - GYRO_OFFSET_Z;
}

void gyro_setup_tick() {
    int TEMP = 0, DEM = 0;
    int TIME = 0, TIME_NEW = 0;
    TIME = rtc.getSeconds();
    for (int i = 0; i < 5; i++)
    {
        while (TIME_NEW - TIME != 1)
        {
            TIME_NEW = rtc.getSeconds();
            get_accel();
            DEM += 1;
        }
        TEMP += DEM;
        DEM = 0;
        TIME = rtc.getSeconds();
    }
    gyro_tick = TEMP / 5.0;
    gyro_tick = 1.0 / gyro_tick;
    Serial.println(gyro_tick, 10);
    ROLL = PITCH = YAW = 0;
}

bool detect_movement() {
    using namespace GyroSettings;
    return (abs(A_X) > MOVE_POINT || abs(A_Y) > MOVE_POINT || abs(A_Z) > MOVE_POINT);
}

#endif
#include "Arduino.h"
#include "RotateMotor.h"
#include "config_button.h"
#include <SimpleKalmanFilter.h>
#include "PCA9685.h"
#include "ps2_controler.h"

byte vibrate = 0;
int time_base_run = millis();


void setup() {
    Serial.begin(9600);
    delay(100);
    Serial.println("Initializing systems...");
    delay(2000);

    Wire.begin();

    pwmController.resetDevices();       // Resets all PCA9685 devices on i2c line
    pwmController.init();               // Initializes module using default totem-pole driver mode, and default phase balancer
    pwmController.setPWMFrequency(50); // Set PWM freq to 500Hz (default is 200Hz, supports 24Hz to 1526Hz)

    for(int i = 0; i < 16; i++) pwms[i] = 0;
    pwmController.setChannelsPWM(0, 16, pwms);

    Wire.setClock(400000);

    initPS2();
    Div_level();

    Serial.println("Initialization success!");

    time_base_run = millis();

    while(true){
        ps2x.read_gamepad();
        if(ps2x.Button(PSB_SELECT) && ps2x.Button(PSB_START)) break;
    }
}

int time_base = 0;
int time_play = 0;

void loop() {
    
    // vibrate = ps2x.Analog(PSAB_CROSS);
    using namespace  ControlState;

    ps2x.read_gamepad();

    // if (millis() - time_base_run > 1){
        reset_motor();
        time_base = millis();

        time_base_run = millis();
        controlCollector();

        positionOfJoystick(console_x_axis, console_y_axis);

        x_axis = console_x_axis;
        y_axis = console_y_axis;

        consoleRead();
        if (fast_stop && !left_power && !right_power) fast_stop = false;
        motorControl();
        
        rotate_all_thing();
        safety_check();
        resetMotionState();
        
        // for(uint16_t i : pwms){
        //     Serial.print(i);
        //     Serial.print(", ");
        // }
        // Serial.println("");

        pwmController.setChannelsPWM(0, 16, pwms);
        delay(10);
    // }

}

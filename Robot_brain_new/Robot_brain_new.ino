#include "RotateMotor.h"
#include "config_button.h"
#include <SimpleKalmanFilter.h>
#include "PCA9685.h"
#include "ps2_controler.h"

void setup() {
    Serial.begin(115200);
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
}

int time_base = 0;
int time_play = 0;

void loop() {
    reset_motor();
    time_base = millis();

    using namespace ControlState;

    ps2x.read_gamepad();
    delay(10);

    controlCollector();

    positionOfJoystick(console_x_axis, console_y_axis);

    x_axis = console_x_axis;
    y_axis = console_y_axis;

    consoleRead();

    motorControl();

    if (fast_stop) fast_stop = false;
    resetMotionState();

    rotate_all_thing();

    safety_check();

    pwmController.setChannelsPWM(0, 16, pwms);

    delay(10);
}

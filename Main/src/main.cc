#include <memory>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include <adafruit.hpp>

const unsigned servoMin = 125;  // Min pulse length out of 4096
const unsigned servoMax = 535;  // Max pulse length out of 4096

int main (void) {
    std::shared_ptr<AdafruitServoDriver> servoHat = nullptr;
    try
    {
        servoHat = std::make_shared<AdafruitServoDriver>(0x40);
    }
    catch (std::runtime_error e)
    {
        std::cerr<<"Failed to create Adafruit Servo Driver.";
        std::cerr<<e.what();
        return 1;
    }

    try
    {
        servoHat->SetPWMFreq(50);
        for (int i = 0; i < 3; i++)
        {
          servoHat->SetPWM(0, 0, servoMin);
          sleep(1);
          servoHat->SetPWM(0, 0, servoMin + ((servoMax - servoMin) / 2));
          sleep(1);
          servoHat->SetPWM(0, 0, servoMax);
          sleep(1);
        }
    }
    catch (std::runtime_error e)
    {
        std::cerr<<e.what();
    }

    return 0;
}

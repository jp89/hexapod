#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include <adafruit.h>

const unsigned servoMin = 125;  // Min pulse length out of 4096
const unsigned servoMax = 535;  // Max pulse length out of 4096

int main (void) {
    AdafruitServoHat servoHat = AdafruitServoHat_Create();

    if (!AdafruitServoHat_Init(servoHat))
    {
      std::cout<<"\nFailed to initialize servo hat!\n";
    }
    else
    {
      std::cout<<"\nInitialized servo hat properly!\n";
    }

    AdafruitServoHat_SetPWMFreq(servoHat, 50);
    for (int i = 0; i < 5; i++)
    {
      AdafruitServoHat_SetPWM(servoHat, 0, 0, servoMin);
      sleep(1);
      AdafruitServoHat_SetPWM(servoHat, 0, 0, servoMin + ((servoMax - servoMin) / 2));
      sleep(1);
      AdafruitServoHat_SetPWM(servoHat, 0, 0, servoMax);
      sleep(1);
    }
    AdafruitServoHat_Deinit(servoHat);
    AdafruitServoHat_Destroy(servoHat);
}

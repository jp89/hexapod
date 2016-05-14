#include <memory>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include <servo_driver.hpp>

const __u16 servoMin = 120;  // Min pulse length out of 4096
const __u16 servoMax = 545;  // Max pulse length out of 4096

int main (void) {
//  std::shared_ptr<AdafruitServoDriver> servoHat = nullptr;
//  try
//  {
//    servoHat = std::make_shared<AdafruitServoDriver>(0x40);
//  }
//  catch (std::runtime_error e)
//  {
//    std::cerr<<"Failed to create Adafruit Servo Driver.";
//    std::cerr<<e.what();
//    return 1;
//  }


//  try
//  {
//    servoHat->SetPWMFreq(50);
//    for (int i = 0; i < 1; i++)
//    {
//      servoHat->SetPWM(0, 0, servoMin);
//      sleep(1);
//      servoHat->SetPWM(0, 0, servoMin + ((servoMax - servoMin) / 2));
//      sleep(1);
//      servoHat->SetPWM(0, 0, servoMax);
//      sleep(1);
//    }
//  }
//  catch (std::runtime_error e)
//  {
//    std::cerr<<e.what();
//  }
  std::vector<__u8> addr = { 0x40 };
  AdafruitServoController controller(1, addr, servoMin, servoMax);

  for (int i = 0; i < 1; i++)
  {
    controller.setServo(0, 0);
    sleep(1);
    controller.setServo(0, 90);
    sleep(1);
    controller.setServo(0, 180);
    sleep(1);
  }


  return 0;
}

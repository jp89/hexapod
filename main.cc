#include <memory>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include <servo_controller.hpp>

const __u8 id = 0;

const __u8 pos_min = 1;
const __u8 pos_mid = 45;
const __u8 pos_max = 90;

int main (void) {
  try {


    AdafruitServoController& controller = AdafruitServoController::getInstance();
    for (int i = 1; i < 180; i++)
    {
      controller.setServo(0, i);
      usleep(50000);
    }
  }
  catch (std::runtime_error& e)
  {
    std::cerr<<"Got runtime error: "<<e.what()<<std::endl;
  }

  return 0;
}

#include <memory>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include <servo_controller.hpp>

const __u8 id = 0;

const __u8 pos_min = 0;
const __u8 pos_mid = 90;
const __u8 pos_max = 180;

int main (void) {
  try {


    AdafruitServoController& controller = AdafruitServoController::getInstance();
//      controller.setServo(id, pos_min);
    for (int i = 0; i < 1; i++)
    {
      controller.setServo(id, pos_min);
      sleep(2);
      controller.setServo(id, pos_mid);
      sleep(2);
      controller.setServo(id, pos_max);
      sleep(2);
    }
  }
  catch (std::runtime_error& e)
  {
    std::cerr<<"Got runtime error: "<<e.what()<<std::endl;
  }

  return 0;
}

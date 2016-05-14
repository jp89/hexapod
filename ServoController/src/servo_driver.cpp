#include "servo_driver.hpp"

static const __u8 PWM_FREQ = 50;
static const __u8 SERVOS_PER_DRIVER = 16;

AdafruitServoController::AdafruitServoController(__u8 modulesCnt, std::vector<__u8> adresses, __u16 servoPulseMin, __u16 servoPulseMax)
 : pulseMin (servoPulseMin), pulseMax(servoPulseMax)
{
  for (int i = 0; i < modulesCnt; i++)
  {
    AdafruitServoDriver* newDriver;
    try {
      newDriver = new AdafruitServoDriver(adresses[i]);
      newDriver->setPWMFreq(PWM_FREQ);
    }
    catch (std::runtime_error& e)
    {
      throw e;
    }
  }
}

AdafruitServoController::~AdafruitServoController()
{
  // TODO add implementation of stopping servos
  for (AdafruitServoDriver* driver : this->drivers)
  {
    delete driver;
  }
}

void AdafruitServoController::setServo(__u8 id, __u8 angle, __u8 speed)
{
  // select driver
  const __u8 selector = id % (SERVOS_PER_DRIVER - 1);
  AdafruitServoDriver* driver = this->drivers.at(selector);
  // calculate angle
  const __u8 servoId = id - (selector * SERVOS_PER_DRIVER);
  driver->setPWM(servoId, 0, this->calculateAngle(angle));
  // set speed
}

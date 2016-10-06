#include <servo_controller.hpp>
//TODO delete this
#include <iostream>
#include <unistd.h>
#include <logging.h>

#define DEBUG

//SET_LOG_LEVEL_NORMAL

static const uint8_t PWM_FREQ = 50;
static const uint8_t SERVOS_PER_DRIVER = 16;

AdafruitServoController::AdafruitServoController()
{
  for (const auto& addr : this->addresses)
    {
      try {
        unique_ptr<AdafruitServoDriver> newDriver (new AdafruitServoDriver(addr));
        newDriver->setPWMFreq(PWM_FREQ);
        this->drivers.push_back( std::move(newDriver) );
      }
      catch (std::runtime_error& e)
      {
        throw e;
      }
    }
}

AdafruitServoController::~AdafruitServoController()
{
}

void AdafruitServoController::setServo(uint8_t id, uint8_t angle, uint8_t speed)
{
  if (id > this->drivers.size() * SERVOS_PER_DRIVER) {
      std::cerr << "Trying to set servo with id bigger than " << std::to_string(id * this->drivers.size()) << std::endl;
      return;
    }
  if (angle > 180)
    angle = 180;
  uint16_t pulse = this->angleToPulse(angle);
  std::pair<uint8_t, uint8_t> driverAndServoId = getDriverAndServoId(id);
  this->drivers[driverAndServoId.first]->setPWM(driverAndServoId.second, 0, pulse);
}

std::pair<uint8_t, uint8_t> AdafruitServoController::getDriverAndServoId(uint8_t id) const
{
  const uint8_t driverId = id / (SERVOS_PER_DRIVER - 1);
  const uint8_t servoId = id - (driverId * SERVOS_PER_DRIVER);
  std::cerr << "Driver id: " << driverId << " servoId: " << servoId << std::endl;
  return {driverId, servoId};
}


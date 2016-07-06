#include <servo_controller.hpp>
//TODO delete this
#include <iostream>
#include <unistd.h>

#define DEBUG

static const __u8 PWM_FREQ = 50;
static const __u8 SERVOS_PER_DRIVER = 16;

AdafruitServoController::AdafruitServoController()
  : running(true), workerThread(&AdafruitServoController::handleServosThread, this)
{
  for (auto it = this->addresses.begin(); it != this->addresses.end(); it++)
  {
    AdafruitServoDriver* newDriver;
    try {
      newDriver = new AdafruitServoDriver(*it);
      newDriver->setPWMFreq(PWM_FREQ);
      this->drivers.push_back(newDriver);
    }
    catch (std::runtime_error& e)
    {
      throw e;
    }
  }
}

AdafruitServoController::~AdafruitServoController()
{
  this->running = false;
  this->workerThread.join();
  // TODO add implementation of stopping servos
  for (AdafruitServoDriver* driver : this->drivers)
  {
    delete driver;
  }
}

const __u8 AdafruitServoController::getDriverIdForServoId(__u8 id)
{
  const __u8 selector = id % (SERVOS_PER_DRIVER - 1);
  const __u8 servoId = id - (selector * SERVOS_PER_DRIVER);
  return servoId;
}

void AdafruitServoController::setServo(__u8 id, __u8 angle, __u8 speed)
{
  const __u8 servoId = getDriverIdForServoId(id);
  this->scheduledMovementsMutex.lock();
  this->scheduledMovements.push_back(ServoMovementParams(servoId, this->calculateAngle(angle), speed));
  this->scheduledMovementsMutex.unlock();
#ifdef DEBUG
  std::cout<<"Scheduled movement "<<ServoMovementParams(servoId, this->calculateAngle(angle), speed).toString()<<std::endl;
#endif
}

void AdafruitServoController::handleServosThread (void)
{
  while (this->running) {
    while (this->scheduledMovements.size())
    {
      this->scheduledMovementsMutex.lock();
      ServoMovementParams mov = this->scheduledMovements.front();
      scheduledMovements.pop_front();
      this->scheduledMovementsMutex.unlock();
      ServoMovement* newJob = new ServoMovement(mov);
      this->movements.push_back(newJob);
#ifdef DEBUG
      std::cout<<"Created job for movement: "<<mov.toString()<<std::endl;
#endif
    }
    while (this->movements.size())
    {
      ServoMovement* mov = this->movements.front();

      if (mov->isFinished())
      {
        this->movements.pop_front();
#ifdef DEBUG
      std::cout<<"Deleting finished job."<<std::endl;
#endif
        mov->sync();
        delete mov;
      }
    }
  }
}

AdafruitServoController::ServoMovementParams::ServoMovementParams(__u8 servoId_, __u16 pulse_, __u8 speed_) :
  servoId(servoId_), pulse(pulse_), speed(speed_) {}

std::string AdafruitServoController::ServoMovementParams::toString (void)
{
  return std::string("Servo id : " + std::to_string(servoId) +
                     ", pulse: " + std::to_string(pulse) +
                       ", speed: " + std::to_string(speed));
}

AdafruitServoController::ServoMovement::ServoMovement(AdafruitServoController::ServoMovementParams move_) : movementParams(move_), finished(false)
{
  this->performMovementThread = new std::thread(&AdafruitServoController::ServoMovement::performMovement, this);
}

AdafruitServoController::ServoMovement::~ServoMovement()
{
  delete this->performMovementThread;
}

bool AdafruitServoController::ServoMovement::isFinished()
{
  return this->finished;
}

void AdafruitServoController::ServoMovement::sync()
{
  this->performMovementThread->join();
}

void AdafruitServoController::ServoMovement::markAsFinished()
{
  this->finished = true;
}

void AdafruitServoController::ServoMovement::performMovement()
{
  AdafruitServoController& controller = AdafruitServoController::getInstance();
  controller.driversMutex.lock();
  AdafruitServoDriver* driver = controller.drivers.at(AdafruitServoController::getDriverIdForServoId(this->movementParams.servoId));
  driver->setPWM(this->movementParams.servoId, 0, this->movementParams.pulse);
  controller.driversMutex.unlock();
  this->markAsFinished();
}

#include <servo_controller.hpp>
//TODO delete this
#include <iostream>
#include <unistd.h>
#include <logging.h>
#include <pthread.h>

static const uint8_t SERVOS_PER_DRIVER = 16;

AdafruitServoController::AdafruitServoController()
{

}

AdafruitServoController::~AdafruitServoController()
{
    this->run = false;
    this->servosThread.join();
}

void AdafruitServoController::Init(vector<uint8_t> driversAddresses_,
                                   uint8_t pwmFreq_, uint16_t pulseMin_,
                                   uint16_t pulseMax_)
{
    this->addresses = driversAddresses_;
    this->pwmFreq = pwmFreq_;
    this->pulseMax = pulseMax_;
    this->pulseMin = pulseMin_;
    for (const auto& addr : this->addresses)
    {
        try
        {
            unique_ptr<AdafruitServoDriver> newDriver (
                        new AdafruitServoDriver(addr));
            newDriver->setPWMFreq(this->pwmFreq);
            this->drivers.push_back( std::move(newDriver) );
        }
        catch (std::runtime_error& e)
        {
            throw e;
        }
    }
    const uint8_t servosCount = (this->addresses.size() * SERVOS_PER_DRIVER) - 1; // -1 is for zero-indexing
    this->servos.reserve(servosCount);
    for (uint8_t i = 0; i < servosCount; i++)
    {
        std::pair<uint8_t, uint8_t> driverAndServoId = getDriverAndServoId(i);
        Servo servo (driverAndServoId.first, driverAndServoId.second, 0, 0, 0);
        // FIXME SUPER CRAZY HACK
        // set all servos initially at 90*
        servo.currentPulse = this->angleToPulse(90);
        servo.desiredPulse = servo.currentPulse; // don't trigger another servo job in servo thread
        this->drivers[servo.driverId]->setPWM(servo.servoId, 0,
                                              servo.currentPulse);
        this->servos.push_back(servo);
    }
    this->run = true;
    this->servosThread = std::thread([this] { this->handleServos(); });
    //    sched_param sch_params;
    //    sch_params.sched_priority = sched_get_priority_max(SCHED_FIFO);
    //    pthread_setschedparam(this->servosThread.native_handle(), SCHED_FIFO,
    //                          &sch_params);
}

void AdafruitServoController::setServo(uint8_t id, uint16_t angle, uint8_t speed)
{
    if (id > this->drivers.size() * SERVOS_PER_DRIVER)
    {
        std::cerr << "Trying to set servo with id bigger than " <<
                     std::to_string(id * this->drivers.size()) << std::endl;
        return;
    }
    if (angle > 180)
    {
        angle = 180;
        std::cerr << __PRETTY_FUNCTION__ << " Trying to set servo to angle"
                                            "greater than 180 degrees."
                  << std::endl;
    }
    else if (angle == 0)
    {
        angle = 1;
        std::cerr << __PRETTY_FUNCTION__ << " Trying to set servo to  0 angle"
                  << std::endl;
    }
    if (speed > 100)
    {
        speed = 100;
        std::cerr << __PRETTY_FUNCTION__ << " Trying to set servo's speed above"
                                            "100 %." << std::endl;
    }
    else if (speed == 0)
    {
        std::cerr << __PRETTY_FUNCTION__ << " Trying to set servo's speed to 0."
                  << std::endl;
        speed = 1;
    }

    uint16_t desiredPulse = this->angleToPulse(angle);

    // schedule job for servo thread
    this->servosMutex.lock();
    if (this->servos[id].desiredPulse != desiredPulse)
    {
        this->servos[id].desiredPulse = desiredPulse;
    }
    else
    {
        this->servosMutex.unlock();
        return;
    }
    // abs (desiredPulse - currentPulse) for unsigned ints
    uint16_t pulseDiff;
    if (this->servos[id].desiredPulse > this->servos[id].currentPulse)
    {
        pulseDiff = this->servos[id].desiredPulse -
                this->servos[id].currentPulse;
    }
    else
    {
        pulseDiff = this->servos[id].currentPulse -
                this->servos[id].desiredPulse;
    }
    // pulseDiff - 100%
    //    x      - speed[%]
    this->servos[id].rate = (pulseDiff * speed) / 100;
    this->servosMutex.unlock();
}

std::pair<uint8_t, uint8_t> AdafruitServoController::getDriverAndServoId(
        uint8_t id) const
{
    const uint8_t driverId = id / (SERVOS_PER_DRIVER - 1);
    const uint8_t servoId = id - (driverId * SERVOS_PER_DRIVER);
    return {driverId, servoId};
}

void AdafruitServoController::handleServos()
{
    const uint32_t MILI_TO_MICRO_SECONDS = 100000;
    uint16_t sleepInterval = (1.0 / static_cast<double> (this->pwmFreq))
            * MILI_TO_MICRO_SECONDS;
    while (this->run) {
        this->servosMutex.lock();
        for (auto& servo : this->servos)
        {
            if (servo.currentPulse != servo.desiredPulse)
            {
                if (servo.currentPulse < servo.desiredPulse)
                {
                    servo.currentPulse += servo.rate;
                    if (servo.currentPulse > servo.desiredPulse)
                        servo.currentPulse = servo.desiredPulse;
                }
                else
                {
                    servo.currentPulse -= servo.rate;
                    if (servo.currentPulse < servo.desiredPulse)
                        servo.currentPulse = servo.desiredPulse;
                }
                this->drivers[servo.driverId]->setPWM(servo.servoId, 0,
                                                      servo.currentPulse);
            }
        }
        this->servosMutex.unlock();
        usleep(sleepInterval);
    }
}


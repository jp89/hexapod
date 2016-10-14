#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include <adafruit_servohat_driver.hpp>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <functional>

using std::vector;
using std::unique_ptr;
using std::thread;
using std::mutex;


/**
 * @brief The AdafruitServoController class provides an interface for controlling digital servo
 * with open-loop-ed (sort of) controll of their speed (in percents, from 0 to 100).
 *
 * This class utilizes:
 * - AdafruitServoDriver - for accessing servos connected to Adafruit Servo Hat board
 * - ServoMovement - for controoling single "movement" of a given servo (eg. from 0 to 90 degrees)
 * - ServoMovementParams - helper structure containing parameters of given movement
 *
 * General idea is to simplify user's API to maxium.
 * All user needs to do is:
 * - get instance of AdafruitServoController (it's a singletion class)
 * - scheduler movement of given servos
 * That's it. Example:
 *
 *     AdafruitServoController& controller = AdafruitServoController::getInstance();
 *     controller.setServo(0, 90);
 *
 * This will initialize AdafruitServoContoller and set first servo on first driver connected to the 90 degrees.
 *
 *
 * TODO: add delay based speed controll
 *
 */
class  AdafruitServoController
{
private:
    vector<unique_ptr<AdafruitServoDriver> > drivers; /**< vector of handles to adafruit servo hat drivers */

    uint16_t pulseMin; /**< minimum pulse width used to drive single servo */
    uint16_t pulseMax; /**< maximum, as above */
    vector<uint8_t> addresses; /**< vector of addresses of servo hat modules */
    uint8_t pwmFreq;
    thread servosThread;
    mutex servosMutex;
    bool run;

    /**
    * @brief AdafruitServoDriver creates easy to use interface for controlling multiple servos using multiple boards.
    */
    AdafruitServoController();

    /**
    * @brief calculateAngle assuming servo can take position between 1 and 180 degrees
    * this method calculates pulse width thta's corresponding to given angle
    * @param angle in degress (1-180)
    * @return pulse width corresponding to the demanded angle
    */
    uint16_t angleToPulse(uint8_t angle) const
    {
        return (((angle - 1) * (this->pulseMax - this->pulseMin)) / (180 - 1)) + this->pulseMin;
    }
    /**
     * @brief getDriverIdForServoId returns ID of servo driver that controlls given servo.
     * Example: let's assume that one driver may controll 16 servos. If we want to access
     * 30th servo method will calculate that it's controlled by 2nd driver and will return 1 (assuming
     * that drivers ar 0-indexed).
     * @param id id of servo
     * @return id of driver controlling servo
     */
    std::pair<uint8_t, uint8_t> getDriverAndServoId(uint8_t id) const;


    // speed controll stuff


    void handleServos(void);

    struct Servo {
        uint8_t driverId;
        uint8_t servoId;
        uint16_t currentPulse;
        uint16_t desiredPulse;
        uint16_t rate;
        Servo(uint8_t driverId_, uint8_t servoId_, uint16_t currentPulse_,
              uint16_t desiredPulse_, uint16_t rate_) :
            driverId(driverId_), servoId(servoId_), currentPulse(currentPulse_),
            desiredPulse(desiredPulse_), rate(rate_) {}
    };

    vector<Servo> servos;
public:
    /**
     * @brief getInstance getter method
     * @return controller instance
     */
    static AdafruitServoController& getInstance(void)
    {
        static AdafruitServoController controller;
        return controller;
    }
    AdafruitServoController (AdafruitServoController const&) = delete;
    void operator=(AdafruitServoController const&)  = delete;

    /**
    * @brief Detor
    */
    ~AdafruitServoController();

    void Init(vector<uint8_t> driversAddresses_, uint8_t pwmFreq_,
              uint16_t pulseMin_, uint16_t pulseMax_);

    /**
    * @brief SetServo sets id-th servo in given angle
    * @param id of servo, assumption is that servo 0-15 are conencted to 1st module, 16-31 to the 2nd etc.
    * @param angle 1-180 degrees
    * @param speed 1-100 % TODo add non-blocking implementation using std::thread
    */
    void setServo(uint8_t id, uint16_t angle, uint8_t speed = 1);
};

#endif // SERVO_DRIVER_H

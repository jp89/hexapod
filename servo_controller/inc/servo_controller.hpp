#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include <adafruit_servohat_driver.hpp>
#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

static const uint8_t SERVO_MIN = 150;  // Min pulse length out of 4096
static const uint16_t SERVO_MAX = 450;  // Max pulse length out of 4096
static const vector<uint8_t> ADDR = { 0x40 };

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
    vector<unique_ptr<AdafruitServoDriver> > drivers; /**< vector of handles to adafruit servo hat drivers */
    const uint16_t pulseMin = SERVO_MIN; /**< minimum pulse width used to drive single servo */
    const uint16_t pulseMax = SERVO_MAX; /**< maximum, as above */
    const vector<uint8_t> addresses = ADDR; /**< vector of addresses of servo hat modules */

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
    /**
    * @brief SetServo sets id-th servo in given angle
    * @param id of servo, assumption is that servo 0-15 are conencted to 1st module, 16-31 to the 2nd etc.
    * @param angle 1-180 degrees
    * @param speed 1-100 % TODo add non-blocking implementation using std::thread
    */
    void setServo(uint8_t id, uint8_t angle, uint8_t speed = 100);
private:
    /**
    * @brief AdafruitServoDriver creates easy to use interface for controlling multiple servos using multiple boards.
    */
    AdafruitServoController();

    /**
    * @brief calculateAngle calculates pulse width for given angle
    * @param angle in degress (1-180)
    * @return pulse width corresponding to the angle
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
};

#endif // SERVO_DRIVER_H

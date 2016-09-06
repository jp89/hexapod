#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include <adafruit_servohat_driver.hpp>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <utility>

static const __u8 SERVO_MIN = 120;  // Min pulse length out of 4096
static const __u16 SERVO_MAX = 540;  // Max pulse length out of 4096
static const std::vector<__u8> ADDR = { 0x40 };

/**
 * @brief The AdafruitServoController class provides an interface for controlling digital servo
 * with (sort of) open-loop-ed controll of their speed (in percents, from 0 to 100).
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
 *
 *
 */
class  AdafruitServoController
{
private:
    friend class ServoMovement;

    /**
     * @brief The ServoMovementParams struct includes servo's movement parameters.
     */
    struct ServoMovementParams
    {
        __u8 servoId; /**< servo's id */
        __u16 pulse; /**< pulse length */
        __u8 speed; /**< speed: from 0 to 100% */
        /**
         * @brief ServoMovementParams ctor
         * @param servoId_
         * @param pulse_
         * @param speed_
         */
        ServoMovementParams(__u8 servoId_, __u16 pulse_, __u8 speed_);
        /**
         * @brief toString serializes structure to readable string
         * @return string with servoid, pulse length and speed
         */
        std::string toString (void);
    };

    /**
     * @brief The Job class is a helper class representing single movement "job". It basically cointains
     * handle to thread setting PWM and allows to join and delete thread gracefully.
     */
    class ServoMovement
    {
    private:
        std::thread* performMovementThread; /**< thread handle */
        bool finished; /**< indicator if thread has finished it's job thus may be joined and deleted */
        ServoMovementParams movementParams; /**< meta data about movement to perform */
    public:
        /**
         * @brief ServoMovement ctor
         * @param move_ parameters of movement
         */
        ServoMovement(ServoMovementParams move_);
        ~ServoMovement();
        /**
         * @brief isFinished indicates if thread finished it's movement
         * @return true if thread finished it's job
         */
        bool isFinished();
        /**
         * @brief sync joins the thread
         */
        void sync();
    private:
        /**
         * @brief markAsFinished marks a thread as finished
         */
        void markAsFinished();
        /**
         * @brief performMovement this method performs servo movement
         * sending commands to AdafruitServoDriver and handling delays in case
         * of speed controll
         */
        void performMovement(void);
    };
    std::mutex driversMutex; /**< mutex protecting access to drivers list */
    std::vector<AdafruitServoDriver*> drivers; /**< vector of handles to adafruit servo hat drivers */
    const __u16 pulseMin = SERVO_MIN; /**< minimum pulse width used to drive single servo */
    const __u16 pulseMax = SERVO_MAX; /**< maximum, as above */
    const std::vector<__u8> addresses = ADDR; /**< vector of addresses of servo hat modules */
    std::mutex scheduledMovementsMutex; /**< mutex protecting access to list of scheduled movements */
    std::list<ServoMovementParams> scheduledMovements; /**< scheduler servo movements to be executed by worker thread. Movement is represented by servo id (__u8) and pulse width (__u16). */
    std::list<ServoMovement*> movements; /**< list of thread handling servos movements at given speed */
    std::thread workerThread; /**< worker thread firing and joining other threads handling servos movements */
    bool running; /**< indicator if worker thread should be running or stopped */

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
    void setServo(__u8 id, __u8 angle, __u8 speed = 100);
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
    __u16 calculateAngle(__u8 angle)
    {
        return (((angle - 1) * (this->pulseMax - this->pulseMin)) / (180 - 1)) + this->pulseMin;
    }
    /**
    * @brief handleServos function executed by worker thread
    * TODO add optimization - worker thread should sleep if moves list is empty
    */
    void handleServosThread (void);
    /**
     * @brief getDriverIdForServoId returns ID of servo driver that controlls given servo.
     * Example: let's assume that one driver may controll 16 servos. If we want to access
     * 30th servo method will calculate that it's controlled by 2nd driver and will return 1 (assuming
     * that drivers ar 0-indexed).
     * @param id id of servo
     * @return id of driver controlling servo
     */
    static const __u8 getDriverIdForServoId(__u8 id);
};

#endif // SERVO_DRIVER_H

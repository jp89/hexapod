#ifndef _ADAFRUIT_
#define _ADAFRUIT_

#include <stdexcept>

#include <linux/types.h>
#include <linux/i2c-dev.h>

/**
 *  This peace of code is an attempt to port Adafruit ServoHat driver from Python to C++.
 *  API is pretty similar to the python version. This code uses C++11.
 *  I tried it only on Raspberry Pi 2 using custom toolchain created with crosstools-ng.
 *
 *  In order to initialize servo you need to:
 *  - create an instance of AdafruitServoDriver
 *  - call SetPWMFreq
 *  In order to move servo you just need to call SetPWM.
 *
 *  In general, two classes are defined:
 *  AdafruitServoDriver - this class is used for controlling single Adafruit ServoHat
 *  GweneralI2CCall - this singleton class is used for sending software reset to all ServoHats on I2C bus
 */

class GeneralI2CCall;

class AdafruitServoDriver {
    friend GeneralI2CCall;
private:
    /**
     * @brief The RaspberryPiVersion enumerates all board versions.
     * Values are pretty straightforward.
     */
    enum class RaspberryPiVersion {
        UNKNOWN,
        RPI_1,
        RPI_2,
        RPI_ZERO,
        RPI_3
    };

    int i2cHandle; /**< handle to i2c device file, POSIX functions are used to open, close, write, read */
    int i2cBusNumber; /**< number of i2c bus, differs between raspberry pi versions */
    RaspberryPiVersion piVersion; /**< version of raspberry pi, established based on board revision from cpuinfo file */
    void SetAllPWM(const __u8 on, const __u8 off); /**< sets all PWM outputs, used internally */
    short GetI2CBusNumber(void); /**< returns i2c bus number specific for given raspberry pi version */
    RaspberryPiVersion GetRPiVersion(void); /**< returns raspberry pi version based on revision */

public:
    /**
     * @brief AdafruitServoDriver ctor
     * @param addr  address of I2C device, may be obtained using i2c-tools
     */
    explicit AdafruitServoDriver(const __u8 addr, const __s8 i2cBusNumber_ = -1);
    ~AdafruitServoDriver();
    /**
     * @brief SetPWMFreq sets frequency of output PWM signal.
     * @param newFreq   demanded frequency in Hz
     */
    void SetPWMFreq(unsigned newFreq);
    /**
     * @brief SetPWM allows to set output PWM wave on given channel.
     * @param channel   channel number, should be 0-15
     * @param on        beginning of high state in the output wave
     * @param off       end of high state in the output wave
     */
    void SetPWM(const __u8 channel, const __u16 on, const __u16 off);

public:
    /**
     * This class instances shouldn't be copied.
     */
    AdafruitServoDriver(const AdafruitServoDriver& other) = delete;
    AdafruitServoDriver& operator=(const AdafruitServoDriver& other) = delete;
};


class GeneralI2CCall
{
public:
    /**
     * @brief SoftwareReset
     * Send SoftwareReset signal to all devices connected to i2c bus.
     */
    static void SoftwareReset(void)
    {
        if (i2c_smbus_write_byte(getInstance().handle.i2cHandle, SWRST) < 0)
        {
            const char* msg = "Failed to perform software reset!\n\r";
            throw std::runtime_error("msg");
        }
    }
private:
    static const __u8 SWRST = 0x06;
    AdafruitServoDriver handle;
    GeneralI2CCall() : handle(0x00) {};
    static GeneralI2CCall& getInstance()
    {
        static GeneralI2CCall instance;
        return instance;
    }
public:
    GeneralI2CCall(GeneralI2CCall const&)  = delete;
    void operator=(GeneralI2CCall const&)  = delete;
};



#endif // _ADAFRUIT_

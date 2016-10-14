#ifndef _ADAFRUIT_
#define _ADAFRUIT_

#include <stdexcept>

#include <linux/types.h>
#include <linux/i2c-dev.h>

/**
 *  This piece of code is an attempt to port Adafruit ServoHat driver from
 *  Python to C++. API is pretty much the same as in the python version.
 *  This port utilizes C++11. I tried it only on Raspberry Pi 2 using custom
 *  toolchain created with crosstools-ng.
 *
 *  In order to initialize servo you need to:
 *  - create an instance of AdafruitServoDriver
 *  - call SetPWMFreq
 *  In order to move servo you just need to call SetPWM.
 *
 *  In general, two classes are defined:
 *  AdafruitServoDriver - this class is used for controlling single
 *                        Adafruit ServoHat shield.
 *  GweneralI2CCall - this singleton class is used for sending software reset
 *                    to all ServoHats on I2C bus.
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
    /**< handle to i2c device file */
    int i2cHandle;
    /**< number of i2c bus, differs between raspberry pi versions */
    int i2cBusNumber;
    /**< version of raspberry pi, based on board revision from cpuinfo file */
    RaspberryPiVersion piVersion;
    /**< sets all PWM outputs, used internally */
    void setAllPWM(const __u8 on, const __u8 off);
    /**< returns i2c bus number specific for given raspberry pi version */
    short getI2CBusNumber(void) const;
    /**< returns raspberry pi version based on revision */
    RaspberryPiVersion getRPiVersion(void) const;

public:
    /**
     * @brief AdafruitServoDriver ctor
     * @param addr  address of I2C device, may be obtained using i2c-tools
     */
    explicit AdafruitServoDriver(const __u8 addr,
                                 const __s8 i2cBusNumber_ = -1);
    ~AdafruitServoDriver();
    /**
     * @brief SetPWMFreq sets frequency of output PWM signal.
     * @param newFreq   demanded frequency in Hz
     */
    void setPWMFreq(unsigned newFreq);
    /**
     * @brief SetPWM allows to set output PWM wave on given channel.
     * @param channel   channel number, should be 0-15
     * @param on        beginning of high state in the output wave
     * @param off       end of high state in the output wave
     */
    void setPWM(const __u8 channel, const __u16 on, const __u16 off);

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
    static void softwareReset(void)
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

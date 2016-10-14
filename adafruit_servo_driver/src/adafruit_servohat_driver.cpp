#include <adafruit_servohat_driver.hpp>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <iostream>
#include <fstream>

#include <sstream>
#include <string>
#include <map>

// Registers - based on Adafruit_PWM_Servo_Driver
const __u8 __MODE1__               = 0x00;
const __u8 __MODE2__               = 0x01;
const __u8 __SUBADR1__             = 0x02;
const __u8 __SUBADR2__             = 0x03;
const __u8 __SUBADR3__             = 0x04;
const __u8 __PRESCALE__            = 0xFE;
const __u8 __LED0_ON_L__           = 0x06;
const __u8 __LED0_ON_H__           = 0x07;
const __u8 __LED0_OFF_L__          = 0x08;
const __u8 __LED0_OFF_H__          = 0x09;
const __u8 __ALL_LED_ON_L__        = 0xFA;
const __u8 __ALL_LED_ON_H__        = 0xFB;
const __u8 __ALL_LED_OFF_L__       = 0xFC;
const __u8 __ALL_LED_OFF_H__       = 0xFD;

// bits - based on Adafruit_PWM_Servo_Driver
const __u8 __RESTART__             = 0x80;
const __u8 __SLEEP__               = 0x10;
const __u8 __ALLCALL__             = 0x01;
const __u8 __INVRT__               = 0x10;
const __u8 __OUTDRV__              = 0x04;

void SleepMilliseconds(unsigned milliseconds);

AdafruitServoDriver::AdafruitServoDriver(const __u8 addr,
                                         const __s8 i2cBusNumber_) :
    i2cBusNumber(i2cBusNumber_)
{
    this->piVersion = getRPiVersion();

    if (this->i2cBusNumber < 0)
    {
        this->i2cBusNumber = getI2CBusNumber();
    }
    if (this->i2cBusNumber < 0)
    {
        throw std::runtime_error("Couldn't get I2C bus number.");
    }

    char filename[20];
    snprintf(filename, 19, "/dev/i2c-%d", i2cBusNumber);
    if ((this->i2cHandle = open(filename, O_RDWR)) < 0)
    {
        std::stringstream ss;
        ss << "Couldn't open " << filename;
        throw std::runtime_error(ss.str());
    }
    if (ioctl(this->i2cHandle, I2C_SLAVE, addr) < 0)
    {
        std::stringstream ss;
        ss << "Couldn't access i2c device on address " << addr;
        throw std::runtime_error(ss.str());
    }

    std::cout<<"Reseting PCA9685 MODE1 (without SLEEP) and MODE2"<<std::endl;
    try
    {
        this->setAllPWM(0, 0);
    }
    catch (std::runtime_error e)
    {
        throw;
    }
    __u8 res;
    res &= i2c_smbus_write_byte_data(this->i2cHandle, __MODE2__, __OUTDRV__);
    res &= i2c_smbus_write_byte_data(this->i2cHandle, __MODE1__, __ALLCALL__);
    SleepMilliseconds(5); // wait for oscillator
    __u8 mode1 = i2c_smbus_read_byte_data(this->i2cHandle, __MODE1__);
    mode1 &= ~__SLEEP__; // wake up (reset sleep)
    res &= i2c_smbus_write_byte_data(this->i2cHandle, __MODE1__, mode1);
    SleepMilliseconds(5); // wait for oscillator
    if (res < 0 || mode1 < 0)
    {
        goto err;
    }
    std::cout<<"I2c initialized successfully!"<<std::endl;
    return;

err:
    close(this->i2cHandle);
    throw std::runtime_error(strerror(errno));
}


AdafruitServoDriver::~AdafruitServoDriver(void)
{
    close(this->i2cHandle);
}

AdafruitServoDriver::RaspberryPiVersion AdafruitServoDriver::getRPiVersion(
        void) const
{
    std::ifstream cpuinfoFile;
    cpuinfoFile.open("/proc/cpuinfo");
    if (!cpuinfoFile.good())
    {
        std::cerr<<"Couldn't open cpuinfo file."<<std::endl;
        return RaspberryPiVersion::UNKNOWN;
    }

    std::multimap<const char*, RaspberryPiVersion> revisionToVersion =
    {
        {"0002",	RaspberryPiVersion::RPI_1},
        {"0003",	RaspberryPiVersion::RPI_1},
        {"0004",	RaspberryPiVersion::RPI_1},
        {"0005",	RaspberryPiVersion::RPI_1},
        {"0006",	RaspberryPiVersion::RPI_1},
        {"0007",	RaspberryPiVersion::RPI_1},
        {"0008",	RaspberryPiVersion::RPI_1},
        {"0009",	RaspberryPiVersion::RPI_1},
        {"000d",	RaspberryPiVersion::RPI_1},
        {"000e",	RaspberryPiVersion::RPI_1},
        {"000f",	RaspberryPiVersion::RPI_1},
        {"0010",	RaspberryPiVersion::RPI_1},
        {"0011",	RaspberryPiVersion::RPI_1},
        {"0012",	RaspberryPiVersion::RPI_1},
        {"0013",	RaspberryPiVersion::RPI_1},
        {"a01041",	RaspberryPiVersion::RPI_2},
        {"a21041",	RaspberryPiVersion::RPI_2},
        {"900092",	RaspberryPiVersion::RPI_ZERO}
    };

    const std::string revisionPattern("Revision");
    std::string line;
    RaspberryPiVersion result = RaspberryPiVersion::UNKNOWN;

    while (std::getline(cpuinfoFile, line))
    {
        if(line.find(revisionPattern) == std::string::npos)
        {
            continue;
        }
        for (auto it: revisionToVersion)
        {
            if (line.find(it.first) != std::string::npos)
            {
                // if revision matches return corresponding version
                result = it.second;
                break;
            }
        }
    }
    cpuinfoFile.close();
    return result;
}

short AdafruitServoDriver::getI2CBusNumber(void) const
{
    switch (this->getRPiVersion())
    {
    case RaspberryPiVersion::RPI_1:
        return 0;
    case RaspberryPiVersion::RPI_2:
        return 1;
    case RaspberryPiVersion::UNKNOWN:
    {
        std::cerr<<"Couldn't determine Raspberry Pi version."<<std::endl;
        return -1;
    }
    case RaspberryPiVersion::RPI_3:
    case RaspberryPiVersion::RPI_ZERO:
    {
        std::cerr<<"This raspberry pi version is not supported yet."<<std::endl;
        return -1;
    }
    default:
        return -1;
    }
}

void SleepMilliseconds(unsigned milliseconds)
{
    struct timespec t;
    t.tv_sec = milliseconds / 1000;
    t.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&t, NULL);
}

void AdafruitServoDriver::setAllPWM(__u8 on , __u8 off)
{
    bool res = true;
    res &= i2c_smbus_write_byte_data(this->i2cHandle,__ALL_LED_ON_L__,
                                     on & 0xFF);
    res &= i2c_smbus_write_byte_data(this->i2cHandle,__ALL_LED_ON_H__, on >> 8);
    res &= i2c_smbus_write_byte_data(this->i2cHandle,__ALL_LED_OFF_L__,
                                     off & 0xFF);
    res &= i2c_smbus_write_byte_data(this->i2cHandle,__ALL_LED_OFF_H__,
                                     off >> 8);
    if (res < 0)
    {
        throw std::runtime_error("I2C transaction failed.");
    }
}

void AdafruitServoDriver::setPWMFreq(unsigned newFreq)
{
    bool res = true;
    // 25MHz / (2^12 * new frequency )
    int prescale = ((25000000.0 / (4096.0 * (double) newFreq)) - 1.0) + 0.5;
    std::cout << "Setting PWM frequency to " << newFreq <<
                 ". Esimated prescale value is: " << prescale<< std::endl;
    __u8 oldMode = i2c_smbus_read_byte_data(this->i2cHandle, __MODE1__);
    __u8 newMode = (oldMode & 0x7F) | 0x10; // sleep
    res &= i2c_smbus_write_byte_data(this->i2cHandle, __MODE1__, newMode);
    res &= i2c_smbus_write_byte_data(this->i2cHandle, __PRESCALE__, prescale);
    res &= i2c_smbus_write_byte_data(this->i2cHandle, __MODE1__, oldMode);
    SleepMilliseconds(5);
    res &= i2c_smbus_write_byte_data(this->i2cHandle, __MODE1__,
                                     oldMode | 0x80);
    if (res < 0)
    {
        throw std::runtime_error("I2C transaction failed.");
    }
}

void AdafruitServoDriver::setPWM(const __u8 channel, const __u16 on,
                                 const __u16 off)
{
    if (channel > 15)
    {
        throw std::invalid_argument("Channel number should be in range 0-15\n");
    }
    bool res = true;
    res &= i2c_smbus_write_byte_data(this->i2cHandle,
                                     __LED0_ON_L__ + (4*channel), on & 0xFF);
    res &= i2c_smbus_write_byte_data(this->i2cHandle,
                                     __LED0_ON_H__ + (4*channel), on >> 8);
    res &= i2c_smbus_write_byte_data(this->i2cHandle,
                                     __LED0_OFF_L__ + (4*channel), off & 0xFF);
    res &= i2c_smbus_write_byte_data(this->i2cHandle,
                                     __LED0_OFF_H__ + (4*channel), off >> 8);
    if (res < 0)
    {
        throw std::runtime_error("I2C transaction failed.");
    }
}

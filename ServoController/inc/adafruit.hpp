#ifndef _ADAFRUIT_
#define _ADAFRUIT_

#include <linux/types.h>

class AdafruitServoDriver {
  private:
    enum class RaspberryPiVersion {
        UNKNOWN,
        RPI_1,
        RPI_2,
        RPI_ZERO,
        RPI_3
    };

    int I2CHandle;
    int adapter_nr;
    RaspberryPiVersion piVersion;

    void SetAllPWM(const __u8 on, const __u8 off);
    short GetI2CBusNumber(void);
    RaspberryPiVersion GetRPiVersion(void);
    static void SleepMilliseconds(unsigned milliseconds);

public:
    AdafruitServoDriver(const __u8 addr);
    ~AdafruitServoDriver();
    void SetPWMFreq(unsigned newFreq);
    void SetPWM(const __u8 channel, const __u8 on, const __u8 off);
};

#endif // _ADAFRUIT_

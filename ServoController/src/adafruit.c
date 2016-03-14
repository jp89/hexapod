#include <adafruit.h>

#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>

#define VERBOSE

#ifdef VERBOSE
#define LOG(x)  fprintf(stderr, "\n\r%s\n\r", x)
#else
#define LOG
#endif

// Registers - based on Adafruit_PWM_Servo_Driver
#define __MODE1__               0x00
#define __MODE2__               0x01
#define __SUBADR1__             0x02
#define __SUBADR2__             0x03
#define __SUBADR3__             0x04
#define __PRESCALE__            0xFE
#define __LED0_ON_L__           0x06
#define __LED0_ON_H__           0x07
#define __LED0_OFF_L__          0x08
#define __LED0_OFF_H__          0x09
#define __ALL_LED_ON_L__        0xFA
#define __ALL_LED_ON_H__        0xFB
#define __ALL_LED_OFF_L__       0xFC
#define __ALL_LED_OFF_H__       0xFD

// bits - based on Adafruit_PWM_Servo_Driver
#define __RESTART__             0x80
#define __SLEEP__               0x10
#define __ALLCALL__             0x01
#define __INVRT__               0x10
#define __OUTDRV__              0x04

typedef enum {
    RaspberryPiVersion_UNKNOWN = 0,
    RaspberryPiVersion_RPI_1,
    RaspberryPiVersion_RPI_2,
    RaspberryPiVersion_RPI_ZERO,
    RaspberryPiVersion_RPI_3,
} RaspberryPiVersion;

struct AdafruitServoHat_{
  int I2CHandle; // handle to file representing connection with the device via i2c
  RaspberryPiVersion piVersion;
};

bool AdafruitSerboHat_SelfCheck(AdafruitServoHat self)
{
  if (!self)
  {
    char message[100] = {};
    sprintf(message, "%s %s", __PRETTY_FUNCTION__, "self in null!");
    LOG(message);
    return false;
  }
  return true;
}

// MODULE'S PRIVATE FUNCTIONS
bool AdafruitServoHat_SetAllPWM(AdafruitServoHat self, __u8 on, __u8 off);

// i2c wirte functions
bool AdafruitServoHat_WriteByteData(AdafruitServoHat self, __u8 reg, __u8 val);
bool AdafruitServoHat_WriteWordData(AdafruitServoHat self, __u8 reg, __u16 val);
bool AdafruitServoHat_WriteByte(AdafruitServoHat self, __u8 val);
// i2c read functions
bool AdafruitServoHat_ReadByteData(AdafruitServoHat self, __u8 reg, __u8* result);
bool AdafruitServoHat_ReadWordData(AdafruitServoHat self, __u8 reg, __u16* result);
bool AdafruitServoHat_ReadByte(AdafruitServoHat self, __u8* result);
// utils
int8_t AdafruitServoHat_GetPiI2CBusNumber(void);
RaspberryPiVersion AdafruitServoHat_GetPiVersion(void);
void AdafruitServoHat_SleepMilliseconds(unsigned miliseconds);



bool AdafruitServoHat_WriteByteData(AdafruitServoHat self, __u8 reg, __u8 val)
{
  if (!AdafruitSerboHat_SelfCheck(self)) return false;
  if (i2c_smbus_write_byte_data(self->I2CHandle, reg, val))
  {
    LOG(strerror(errno));
    return false;
  }
  return true;
}

bool AdafruitServoHat_WriteWordData(AdafruitServoHat self, __u8 reg, __u16 val)
{
  if (!AdafruitSerboHat_SelfCheck(self)) return false;
  if (i2c_smbus_write_word_data(self->I2CHandle, reg, val))
  {
    LOG(strerror(errno));
    return false;
  }
  return true;
}

bool AdafruitServoHat_WriteByte(AdafruitServoHat self, __u8 val)
{
  if (!AdafruitSerboHat_SelfCheck(self)) return false;
  if (i2c_smbus_write_byte(self->I2CHandle, val))
  {
    LOG(strerror(errno));
    return false;
  }
  return true;
}

bool AdafruitServoHat_ReadByteData(AdafruitServoHat self, __u8 reg, __u8* result)
{
  if (!AdafruitSerboHat_SelfCheck(self)) return false;
  *result = i2c_smbus_read_byte_data(self->I2CHandle, reg);
  if (*result < 0)
  {
    LOG(strerror(errno));
    return false;
  }
  return true;
}

bool AdafruitServoHat_ReadWordData(AdafruitServoHat self, __u8 reg, __u16* result)
{
  if (!AdafruitSerboHat_SelfCheck(self)) return false;
  *result = i2c_smbus_read_word_data(self->I2CHandle, reg);
  if (*result < 0)
  {
    LOG(strerror(errno));
    return false;
  }
  return true;
}

bool AdafruitServoHat_ReadByte(AdafruitServoHat self, __u8* result)
{
  if (!AdafruitSerboHat_SelfCheck(self)) return false;
  *result = i2c_smbus_read_byte(self->I2CHandle);
  if (*result < 0)
  {
    LOG(strerror(errno));
    return false;
  }
  return true;
}

AdafruitServoHat AdafruitServoHat_Create(void)
{
  AdafruitServoHat self = (AdafruitServoHat) malloc(sizeof(AdafruitServoHat));
  self->piVersion = RaspberryPiVersion_UNKNOWN;
  self->I2CHandle = -1;
  return self;
}

void AdafruitServoHat_Destroy(AdafruitServoHat self)
{
  if (!AdafruitSerboHat_SelfCheck(self)) return;
  free(self);
}

bool AdafruitServoHat_Init(AdafruitServoHat self)
{
  if (!AdafruitSerboHat_SelfCheck(self)) return false;
  int adapter_nr = AdafruitServoHat_GetPiI2CBusNumber();
  if (adapter_nr < 0)
  {
    LOG("Unable to determine i2c bus number!");
    return false;
  }

  char filename[20];
  snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
  if ((self->I2CHandle = open(filename, O_RDWR)) < 0)
  {
    goto error;
  }

  int addr = 0x40; /* The I2C address */
  if (ioctl(self->I2CHandle, I2C_SLAVE, addr) < 0)
  {
    goto error;
  }

  LOG("Reseting PCA9685 MODE1 (without SLEEP) and MODE2");
  AdafruitServoHat_SetAllPWM(self, 0, 0);
  if (!AdafruitServoHat_WriteByteData(self, (__u8) __MODE2__, (__u8) __OUTDRV__)) { LOG("Ups! Something went wrong!"); goto error;}
  if (!AdafruitServoHat_WriteByteData(self, __MODE1__, __ALLCALL__)) { LOG("Ups! Something went wrong!"); goto error;}
  AdafruitServoHat_SleepMilliseconds(5); // wait for oscillator

  __u8 mode1;
  if (!AdafruitServoHat_ReadByteData(self, __MODE1__, &mode1)) { LOG("Ups! Something went wrong!"); goto error;}

  mode1 &= ~__SLEEP__; // wake up (reset sleep)
  if (!AdafruitServoHat_WriteByteData(self, __MODE1__, mode1)) { LOG("Ups! Something went wrong!"); goto error;}
  AdafruitServoHat_SleepMilliseconds(5); // wait for oscillator

  LOG("I2c initialized successfully!");
  return true;

error:
  close(self->I2CHandle);
  LOG(strerror(errno));
  return false;
}

bool AdafruitServoHat_Deinit(AdafruitServoHat self)
{
  if (!AdafruitSerboHat_SelfCheck(self)) return false;
  close(self->I2CHandle);
}

RaspberryPiVersion AdafruitServoHat_GetPiVersion(void)
{
  const char* cpuinfo_path = "/proc/cpuinfo";
  FILE* cpuinfo = NULL;
  cpuinfo = fopen(cpuinfo_path, "r");
  if (!cpuinfo)
  {
    LOG("Couldn't open cpuinfo file.");
    return RaspberryPiVersion_UNKNOWN;
  }

  typedef struct
  {
    const char* revision;
    RaspberryPiVersion model;
  } revisionToModelTuple;

  revisionToModelTuple revisionToModelTable[] =
  {
    {"0002",	RaspberryPiVersion_RPI_1},
    {"0003",	RaspberryPiVersion_RPI_1},
    {"0004",	RaspberryPiVersion_RPI_1},
    {"0005",	RaspberryPiVersion_RPI_1},
    {"0006",	RaspberryPiVersion_RPI_1},
    {"0007",	RaspberryPiVersion_RPI_1},
    {"0008",	RaspberryPiVersion_RPI_1},
    {"0009",	RaspberryPiVersion_RPI_1},
    {"000d",	RaspberryPiVersion_RPI_1},
    {"000e",	RaspberryPiVersion_RPI_1},
    {"000f",	RaspberryPiVersion_RPI_1},
    {"0010",	RaspberryPiVersion_RPI_1},
    {"0011",	RaspberryPiVersion_RPI_1},
    {"0012",	RaspberryPiVersion_RPI_1},
    {"0013",	RaspberryPiVersion_RPI_1},
    {"a01041",	RaspberryPiVersion_RPI_2},
    {"a21041",	RaspberryPiVersion_RPI_2},
    {"900092",	RaspberryPiVersion_RPI_ZERO}
  };
  size_t revisionToModelTableLength = sizeof(revisionToModelTable) / sizeof(revisionToModelTuple);

  char* line = NULL;
  const char* revisionPattern = "Revision";
  size_t len = 0;
  ssize_t read;
  RaspberryPiVersion result = RaspberryPiVersion_UNKNOWN;

  while ((read = getline(&line, &len, cpuinfo)) != -1)
  {
    if( !strstr(line, revisionPattern))
    {
      continue;
    }
    size_t i = 0;
    for (i; i < revisionToModelTableLength; i++)
    {
      if (strstr(line, revisionToModelTable[i].revision))
      {
        result = revisionToModelTable[i].model;
        break;
      }
    }
  }

  fclose(cpuinfo);
  if (line)
  {
    free(line);
  }
  return result;
}

int8_t AdafruitServoHat_GetPiI2CBusNumber(void)
{
  switch (AdafruitServoHat_GetPiVersion())
  {
    case RaspberryPiVersion_RPI_1:
      return 0;
    case RaspberryPiVersion_RPI_2:
      return 1;
    case RaspberryPiVersion_UNKNOWN:
      {
        char message[100] = {};
        sprintf(message, "%s %s", __PRETTY_FUNCTION__, "couldn't determine Raspberry Pi version.");
        LOG(message);
        return -1;
      }
    case RaspberryPiVersion_RPI_3:
    case RaspberryPiVersion_RPI_ZERO:
      {
        char message[100] = {};
        sprintf(message, "%s %s", __PRETTY_FUNCTION__, "your Raspberry Pi version is not supported yet.");
        LOG(message);
        return -1;
      }
    default:
      return -1;
  }
}

void AdafruitServoHat_SleepMilliseconds(unsigned milliseconds)
{
    struct timespec t;
    t.tv_sec = milliseconds / 1000;
    t.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&t, NULL);
}

bool AdafruitServoHat_SetAllPWM(AdafruitServoHat self, __u8 on , __u8 off)
{
  if (!AdafruitSerboHat_SelfCheck(self)) return false;
  bool res = true;
  res &= AdafruitServoHat_WriteByteData(self,__ALL_LED_ON_L__, on & 0xFF);
  res &= AdafruitServoHat_WriteByteData(self,__ALL_LED_ON_H__, on >> 8);
  res &= AdafruitServoHat_WriteByteData(self,__ALL_LED_OFF_L__, off & 0xFF);
  res &= AdafruitServoHat_WriteByteData(self,__ALL_LED_OFF_H__, off >> 8);
  return res;
}

bool AdafruitServoHat_SetPWMFreq(AdafruitServoHat self, unsigned newFreq)
{
  if (!AdafruitSerboHat_SelfCheck(self)) return false;
  bool res = true;

  int prescale = ((25000000.0 / (4096.0 * (double) newFreq)) - 1.0) + 0.5; // 25MHz / (2^12 * new frequency )
  LOG("Setting PWM frequency to ");
  char newFreqStr[50];
  sprintf(newFreqStr, "%u", newFreq);
  LOG(newFreqStr);
  LOG("Estimated pre-scale: ");
  char prescaleStr[50];
  sprintf(prescaleStr, "%d", prescale);
  LOG(prescaleStr);
  __u8 oldMode = 0;
  res &= AdafruitServoHat_ReadByteData(self, __MODE1__, &oldMode);
  __u8 newMode = (oldMode & 0x7F) | 0x10; // sleep
  res &= AdafruitServoHat_WriteByteData(self, __MODE1__, newMode); // go to sleep
  res &= AdafruitServoHat_WriteByteData(self, __PRESCALE__, prescale);
  res &= AdafruitServoHat_WriteByteData(self, __MODE1__, oldMode);
  AdafruitServoHat_SleepMilliseconds(5);
  res &= AdafruitServoHat_WriteByteData(self, __MODE1__, oldMode | 0x80);
  return res;
}

bool AdafruitServoHat_SetPWM(AdafruitServoHat self, unsigned channel, unsigned on, unsigned off)
{
  if (!AdafruitSerboHat_SelfCheck(self)) return false;
  bool res = true;
  res &= AdafruitServoHat_WriteByteData(self, __LED0_ON_L__ + (4*channel), on & 0xFF);
  res &= AdafruitServoHat_WriteByteData(self, __LED0_ON_H__ + (4*channel), on >> 8);
  res &= AdafruitServoHat_WriteByteData(self, __LED0_OFF_L__ + (4*channel), off & 0xFF);
  res &= AdafruitServoHat_WriteByteData(self, __LED0_OFF_H__ + (4*channel), off >> 8);
  return res;
}

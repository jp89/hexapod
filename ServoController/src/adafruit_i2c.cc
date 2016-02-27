#include <adafruit_i2c.hh>

#include <iostream>
#include <fstream>

#define VERBOSE
#define CPUINFO_FILE "/proc/cpuinfo"

Adafruit_I2C::Adafruit_I2C()
{

}

~Adafruit_I2C ::Adafruit_I2C()
{

}

static PiRevision getPiRevision()
{
  std::ifstream cpuinfo;
  cpuinfo.open(CPUINFO_FILE);
  if (!cpuinfo.good())

}

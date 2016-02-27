#ifndef ADAFRUIT_I2C_
#define ADAFRUIT_I2C_

#include <vector>

/**
  C++ port of Adafruit_I2C.py module
  */
class Adafruit_I2C {
public:
  enum PiRevision {
    // raspberry_pi0 = 0 for future implementation
      raspberry_pi = 1,
      raspberry_pi2 = 2
  };
  Adafruit_I2C(void);
  ~Adafruit_I2C(void);
  bool Write8(short reg, short value);
  bool Write16(short reg, short value);
  static PiRevision GetPiRevision(void);
  static short GetPiI2CBusNumber(void);

private:
  short ReverseByteOrder(short data);
  void ErrMsg(void);
  short ReadU8(short reg);
  short ReadS8(short  reg);
  int ReadU16(short  reg, bool littleEndian = true);
  int ReadS16(short  reg, bool littleEndian = true);
  bool WriteList(short reg, std::vector<short> list);
  bool ReadList(short reg, std::vector<short> list);
};

#endif // ADAFRUIT_I2C_

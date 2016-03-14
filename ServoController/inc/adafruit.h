#ifndef _ADAFRUIT_
#define _ADAFRUIT_

#include <stdbool.h>
#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AdafruitServoHat_* AdafruitServoHat;

AdafruitServoHat AdafruitServoHat_Create(void);

void AdafruitServoHat_Destroy(AdafruitServoHat self);
bool AdafruitServoHat_Init(AdafruitServoHat self);
bool AdafruitServoHat_Deinit(AdafruitServoHat self);
bool AdafruitServoHat_SetPWMFreq(AdafruitServoHat self, unsigned newFreq);
bool AdafruitServoHat_SetPWM(AdafruitServoHat self, unsigned channel, unsigned on, unsigned off);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _ADAFRUIT_

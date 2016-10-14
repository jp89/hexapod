#ifndef __LEG_IK__
#define __LEG_IK__

#include <stdint.h>

/**
 * @brief The LegIK class implements inverse kinematics of 3 DOF robot leg.
 *
 *                        TOP VIEW
 *                 0*.
 *                   .    /  <-- COXA
 *                   .   /
 *                   .  /  angle1
 *          -------- -/  ......... 180*
 *                   |
 *           robot's |
 *            body   |
 *                   |
 *                   |
 *
 *
 *                      SIDE VIEW
 *                            /\ angle3
 *                   FEMUR->/   \ <- TIBIA
 *                        /       \
 *  robot's body ________/ angle2  \
 *
 *
 */
class LegIK {
private:
    // leg's elements - in mm
    uint16_t coxa;
    uint16_t femur;
    uint16_t tibia;
    // angles - in degrees
    uint16_t angle1;
    uint16_t angle2;
    uint16_t angle3;
    // end-effector's (foot) position
    uint16_t x;
    uint16_t y;
    uint16_t z;
    /**
     * @brief setInitialPosition takes initial position of all agnles
     * calculates position of end-effector and stores it.
     */
    void setInitialPosition(void);
public:
    /**
     * @brief LegIK ctor takes lengths of leg's elements.
     * @param coxa_len in mm
     * @param femur_len in mm
     * @param tibia_len in mm
     */
    LegIK(uint16_t coxa_, uint16_t femur_, uint16_t tibia_, uint16_t angle1_,
          uint16_t angle2_, uint16_t angle3_);

};

#endif // __LEG_IK__

#include <leg_ik.h>
#include <cmath>

void LegIK::setInitialPosition()
{

}

LegIK::LegIK(uint16_t coxa_, uint16_t femur_, uint16_t tibia_, uint16_t angle1_, uint16_t angle2_, uint16_t angle3_)
{
    this->coxa = coxa_;
    this->femur = femur_;
    this->tibia = tibia_;
    this->angle1 = angle1_;
    this->angle2 = angle2_;
    this->angle3 = angle3_;
    setInitialPosition();
}

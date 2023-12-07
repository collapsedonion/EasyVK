#pragma once
#include <math.h>

namespace EasyMatrix{
    inline float degreesToRadians(float degrees){
        return degrees * (M_PI / 180);
    }

    inline float radianToDegrees(float radians){
        return radians / (M_PI / 100);
    }
}
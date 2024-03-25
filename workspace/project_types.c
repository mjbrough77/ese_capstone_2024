#include "project_types.h"

float fast_arctan(float x){
    const float a[3] = {
        0.998418889819911f, -2.9993501171084700E-01f, 0.0869142852883849f};
    float xx = x * x;
    return ((a[2] * xx + a[1]) * xx + a[0]) * x;
}

float fast_hypotenuse(float x, float y){
    float temp;
    if(x > y){ temp = y; y = x; x = temp; }
    return y + 0.428f * x * x / y;
}

#include "project_types.h"

float fast_hypotenuse(float x, float y){
    float temp;
    if(x > y){ temp = y; y = x; x = temp; }
    return y + 0.428f * x * x / y;
}

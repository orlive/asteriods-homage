#include <iostream>
#include <vector>
#include <cmath>

#ifdef NOEXTERN 
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN float bogenmass(float value);
EXTERN float grad(float value);
EXTERN float randomf(float low, float high);
EXTERN int random(int low, int high);

#undef NOEXTERN
#undef EXTERN

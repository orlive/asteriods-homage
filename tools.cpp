#include "tools.h"

const double PI = std::acos(-1);

float bogenmass(float value) {
  return ( PI / 180.0f ) * value;
}

float grad(float value) {
  return value / ( PI / 180.0f );
}

float randomf(float low, float high) {
  if (low > high)
    return high;
  return low + (std::rand() % (int)(high - low + 1));
}

int random(int low, int high) {
  if (low > high)
    return high;
  return low + (std::rand() % (high - low + 1));
}


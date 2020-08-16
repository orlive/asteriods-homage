#ifndef FORCE_DEFINE
#define FORCE_DEFINE

#include "vector.h"

class force : public vector {
  private:
    float m_combinedX = 0.0f;
    float m_combinedY = 0.0f;

  public:
    force( float magnitude=0 , float randomPercentage=0 , int degree=0 );
    ~force();
    void calcCombinedOffsets( float& xAdd,float& yAdd );
    void calcOffsets( float& xAdd,float& yAdd );
    void slowDown();
};

#endif

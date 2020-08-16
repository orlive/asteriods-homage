#ifndef FORCE_DEFINE
#define FORCE_DEFINE

#include "vector.h"

class force : public vector {
  private:
    POINT m_combined = { .x = 0.0f , .y = 0.0f };

  public:
    force( float magnitude=0 , float randomPercentage=0 , int degree=0 );
    ~force();
    void calcCombinedOffsets( POINT& add );
    void calcOffsets( POINT& add );
    void slowDown();
};

#endif

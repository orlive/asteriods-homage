#ifndef LASER_DEFINE
#define LASER_DEFINE

#include "base.h"

class laser : public base {
  private:
    force m_force;
    
  public:
    laser( float x,float y,float degree );
    ~laser();
    int degree();
    void render( SDL_Renderer *renderer );
};

#endif

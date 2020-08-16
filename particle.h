#ifndef PARTICLE_DEFINE
#define PARTICLE_DEFINE

#include "base.h"

class particle : public base {
  private:
    force m_force;

  public:
    particle( int x,int y,float randomPositionOffset,float speedStart,float speedEnd,float degreeStart,float degreeEnd );
    ~particle();
    void move();
    void render( SDL_Renderer *renderer );
};

#endif

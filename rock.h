#ifndef ROCK_DEFINE
#define ROCK_DEFINE

#include "base.h"

class rock : public base {
  private:
    std::shared_ptr<force> m_force;
    float m_degree;
    float m_degreeOffset;

  public:
    rock(int x,int y,float size,float speed,int degree=0);
    ~rock();
    float magnitude();
    float degree();
    void move();
    void render( SDL_Renderer *renderer );
};

#endif

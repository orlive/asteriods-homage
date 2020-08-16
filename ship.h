#ifndef SHIP_DEFINE
#define SHIP_DEFINE

#include "base.h"

class ship : public base {

  private:
    std::vector<int> m_thrustX;
    std::vector<int> m_thrustY;
    force m_force;
    float m_thrustOn = 0.0f;
    float m_degree;
    float m_speed;
    int   m_waitForRenew;
    int   m_waitForViable;

  public:
    ship();
    bool isViable();
    float getDegree();
    bool renew();
    void resize( float size );
    void thrust(std::vector< std::shared_ptr<particle> >& particles);
    void destroy(std::vector< std::shared_ptr<particle> >& particles);
    void stop();
    void setXY(float x=0,float y=0);
    void rotateLeft();
    void rotateRight();
    void moveShip();
    void drawShip( SDL_Renderer *renderer , std::vector<int>& xn , std::vector<int>& yn , const float offsetX , const float offsetY );
    void render( SDL_Renderer *renderer );
};

#endif

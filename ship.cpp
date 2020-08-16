#include "tools.h"
#include "particle.h"
#include "ship.h"

ship::ship() {
  m_x             = 200.0f;
  m_y             = 200.0f;
  m_degree        = 0;
  m_speed         = 0;
  m_waitForViable = 0;

  resize(gameWorld.ship.size);
}

bool ship::isViable() {
  return ( gameWorld.time.lastTicks > m_waitForViable );
}

float ship::getDegree() {
  return m_degree;
}

void ship::renew() {
  if ( this->isDestroyed() && gameWorld.time.lastTicks > m_waitForRenew ) {
    this->m_destroyed = false;
    this->m_waitForViable = gameWorld.time.lastTicks + gameWorld.ship.wait_viable;
std::cout << "this->m_waitForViable:" << this->m_waitForViable << std::endl; 
  }
}

void ship::resize( int size ) {
  int offset = -(size/2);

  clearPolygon();

  addToPolygon(              0  , offset);
  addToPolygon(  size/5*2  , size+offset );
  addToPolygon(-(size/5*2) , size+offset );

  m_thrustX.clear();
  m_thrustY.clear();

  m_thrustX.push_back( size/8*2); m_thrustY.push_back( -offset*1.1f );
  m_thrustX.push_back(-size/8*2); m_thrustY.push_back( -offset*1.1f );
  m_thrustX.push_back(        0); m_thrustY.push_back( -offset*1.7f );
}

void ship::thrust(std::vector< std::shared_ptr<particle> >& particles) {
  if ( m_destroyed ) return;

  m_force.add(m_degree,gameWorld.time.timeFactor * 200.0f);
  m_thrustOn = gameWorld.time.lastTicks + 200;

  float bm = bogenmass(m_degree);
  int x = m_x - gameWorld.ship.size*std::sin(bm);
  int y = m_y + gameWorld.ship.size*std::cos(bm);

  particles.push_back( std::shared_ptr<particle>( new particle( x,y,gameWorld.ship.size,
                                                                gameWorld.ship.thrust_particles.min_speed,
                                                                gameWorld.ship.thrust_particles.max_speed,
                                                                gameWorld.ship.thrust_particles.min_degree + m_degree,
                                                                gameWorld.ship.thrust_particles.max_degree + m_degree ) ) );
}

void ship::destroy(std::vector< std::shared_ptr<particle> >& particles) {
  if ( ! m_destroyed ) {
    m_waitForRenew = gameWorld.time.lastTicks + 1000;
    m_destroyed    = true;
    m_destroyedForces.clear();
    for ( int i=0 ; i<m_polygonX.size() ; i++ ) {
      m_destroyedForces.push_back( std::shared_ptr<force>( new force(20,10,random(0,359)) ) );
    }
    for ( int i=0 ; i<30 ; i++ ) {
      particles.push_back( std::shared_ptr<particle>( new particle( m_x,m_y,gameWorld.ship.size, /*gameWorld.ship.size*/
                                                                    5,         // speed min
                                                                    15,        // speed max
                                                                    0,         // degree min
                                                                    359 ) ) ); // degree max
    }
  }
}

void ship::stop() {
  m_force.stop();
}

void ship::setXY(float x,float y) {
  m_force.setXY(10*x,10*y);
}

void ship::rotateLeft() {
  if ( m_destroyed ) return;
  m_degree -= gameWorld.time.timeFactor * 180;
  while ( m_degree < 0 ) m_degree += 360.0f;
}

void ship::rotateRight() {
  if ( m_destroyed ) return;
  m_degree += gameWorld.time.timeFactor * 180;
  while ( m_degree >= 360 ) m_degree -= 360.0f;
}

void ship::moveShip() {
  float xAdd = 0;
  float yAdd = 0;

  m_force.calcOffsets(xAdd,yAdd);

  m_x += xAdd;
  m_y += yAdd;

  gameWorld.adjustBoundaries( m_x,m_y );

  m_force.slowDown();
}

void ship::drawShip( SDL_Renderer *renderer , std::vector<int>& xn , std::vector<int>& yn , const float offsetX , const float offsetY ) {
  // TODO -> für ein Array an Punkten kann auch eine Funktion in der Base-Class verwendet werden!
  SDL_RenderDrawLine(renderer , m_x+xn[0]+offsetX , m_y+yn[0]+offsetY , m_x+xn[1]+offsetX , m_y+yn[1]+offsetY);
  SDL_RenderDrawLine(renderer , m_x+xn[1]+offsetX , m_y+yn[1]+offsetY , m_x+xn[2]+offsetX , m_y+yn[2]+offsetY);
  SDL_RenderDrawLine(renderer , m_x+xn[2]+offsetX , m_y+yn[2]+offsetY , m_x+xn[0]+offsetX , m_y+yn[0]+offsetY);
}

void ship::render( SDL_Renderer *renderer ) {
  SDL_SetRenderDrawColor(renderer, 255,255,255, SDL_ALPHA_OPAQUE);

  m_transformedX.clear();
  m_transformedY.clear();

  float bm = bogenmass(m_degree);

  if ( this->isDestroyed() ) { 
    for ( int i=0 ; i<m_polygonX.size() ; i++ ) {
      float xAdd = 0;
      float yAdd = 0;

      m_destroyedForces.at(i)->calcOffsets(xAdd,yAdd);

      m_transformedX.push_back( m_polygonX[i]*std::cos(bm) - m_polygonY[i]*std::sin(bm) + m_x + xAdd );
      m_transformedY.push_back( m_polygonX[i]*std::sin(bm) + m_polygonY[i]*std::cos(bm) + m_y + yAdd );
    }
  } else {
    for ( int i=0 ; i<m_polygonX.size() ; i++ ) {
      m_transformedX.push_back( m_polygonX[i]*std::cos(bm) - m_polygonY[i]*std::sin(bm) + m_x );
      m_transformedY.push_back( m_polygonX[i]*std::sin(bm) + m_polygonY[i]*std::cos(bm) + m_y );
    }
  }

  if ( ! this->isViable() ) {
    static bool visible;
    static int last;
    if ( gameWorld.time.lastTicks > last ) {
      visible = not visible;
      last = gameWorld.time.lastTicks + 100;
    }
    if ( ! visible ) return;
  }

  drawObjectWithMirrors( renderer,m_transformedX,m_transformedY );

  if ( m_thrustOn > gameWorld.time.lastTicks ) {
    std::vector<int> xn;
    std::vector<int> yn;
    for ( int i=0 ; i<m_thrustX.size() ; i++ ) {
      int pointY = (i==m_thrustX.size()-1) ? random(2,17) : random(-1,1);
      int pointX = random(-1,1);
      xn.push_back( (pointX+m_thrustX[i])*std::cos(bm) - ( pointY+m_thrustY[i])*std::sin(bm) + m_x );
      yn.push_back( (pointX+m_thrustX[i])*std::sin(bm) + ( pointY+m_thrustY[i])*std::cos(bm) + m_y );
    }
    drawObjectWithMirrors( renderer,xn,yn );
  }
}

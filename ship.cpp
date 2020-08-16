#include "tools.h"
#include "particle.h"
#include "ship.h"

ship::ship() {
  m_position.x    = 200.0f;
  m_position.y    = 200.0f;
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

bool ship::renew() {
  if ( this->isDestroyed() && gameWorld.time.lastTicks > m_waitForRenew ) {
    this->m_destroyed = false;
    this->m_waitForViable = gameWorld.time.lastTicks + gameWorld.ship.wait_viable;
    return true;
  }
  return false;
}

void ship::resize( float size ) {
  float offset = -(size/2);

  clearPolygon();

  addToPolygon( {              0  , offset } );
  addToPolygon( {  size/5*2  , size+offset } );
  addToPolygon( {-(size/5*2) , size+offset } );

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
  int x = m_position.x - gameWorld.ship.size*std::sin(bm);
  int y = m_position.y + gameWorld.ship.size*std::cos(bm);

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
    for ( int i=0 ; i<m_polygon.size() ; i++ ) {
      m_destroyedForces.push_back( std::shared_ptr<force>( new force(20,10,random(0,359)) ) );
    }
    for ( int i=0 ; i<30 ; i++ ) {
      particles.push_back( std::shared_ptr<particle>( new particle( m_position.x,m_position.y,gameWorld.ship.size, /*gameWorld.ship.size*/
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
  m_force.setPosition( { 10*x , 10*y } );
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
  POINT add = { 0.0f , 0.0f };

  m_force.calcOffsets( add );

  m_position.x += add.x;
  m_position.y += add.y;

  gameWorld.adjustBoundaries( m_position );

  m_force.slowDown();
}

void ship::drawShip( SDL_Renderer *renderer , std::vector<int>& xn , std::vector<int>& yn , const float offsetX , const float offsetY ) {
  // TODO -> für ein Array an Punkten kann auch eine Funktion in der Base-Class verwendet werden!
  SDL_RenderDrawLine(renderer , m_position.x+xn[0]+offsetX , m_position.y+yn[0]+offsetY , m_position.x+xn[1]+offsetX , m_position.y+yn[1]+offsetY);
  SDL_RenderDrawLine(renderer , m_position.x+xn[1]+offsetX , m_position.y+yn[1]+offsetY , m_position.x+xn[2]+offsetX , m_position.y+yn[2]+offsetY);
  SDL_RenderDrawLine(renderer , m_position.x+xn[2]+offsetX , m_position.y+yn[2]+offsetY , m_position.x+xn[0]+offsetX , m_position.y+yn[0]+offsetY);
}

void ship::render( SDL_Renderer *renderer ) {
  SDL_SetRenderDrawColor(renderer, 255,255,255, SDL_ALPHA_OPAQUE);

  m_transformed.clear();

  float bm = bogenmass(m_degree);

  if ( this->isDestroyed() ) { 
    for ( int i=0 ; i<m_polygon.size() ; i++ ) {
      POINT add = { 0.0f , 0.0f };

      m_destroyedForces.at(i)->calcOffsets( add );

      m_transformed.push_back( { m_polygon[i].x*std::cos(bm) - m_polygon[i].y*std::sin(bm) + m_position.x + add.x ,
                                 m_polygon[i].x*std::sin(bm) + m_polygon[i].y*std::cos(bm) + m_position.y + add.y } );
    }
  } else {
    for ( int i=0 ; i<m_polygon.size() ; i++ ) {
      m_transformed.push_back( { m_polygon[i].x*std::cos(bm) - m_polygon[i].y*std::sin(bm) + m_position.x ,
                                 m_polygon[i].x*std::sin(bm) + m_polygon[i].y*std::cos(bm) + m_position.y } );
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

  drawObjectWithMirrors( renderer,m_transformed );

  if ( m_thrustOn > gameWorld.time.lastTicks ) {
    std::vector<POINT> xy;
    for ( int i=0 ; i<m_thrustX.size() ; i++ ) {
      float pointY = (i==m_thrustX.size()-1) ? random(2,17) : random(-1,1);
      float pointX = random(-1,1);
      xy.push_back( { (pointX+m_thrustX[i])*std::cos(bm) - ( pointY+m_thrustY[i])*std::sin(bm) + m_position.x ,
                      (pointX+m_thrustX[i])*std::sin(bm) + ( pointY+m_thrustY[i])*std::cos(bm) + m_position.y } );
    }
    drawObjectWithMirrors( renderer,xy );
  }
}

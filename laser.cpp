#include "tools.h"
#include "laser.h"

laser::laser( float x,float y,float degree ) {
  addToPolygon(0,-gameWorld.ship.size/2); // TODO : use SHIP-SIZE
  addToPolygon(0,-gameWorld.ship.size/2 - gameWorld.laser.size);

  m_x = x;
  m_y = y;
  m_force.add(degree,gameWorld.laser.speed);

  float bm = bogenmass(degree);

  for ( int i=0 ; i<m_polygonX.size() ; i++ ) {
    m_transformedX.push_back( m_polygonX[i]*std::cos(bm) - m_polygonY[i]*std::sin(bm) + x );
    m_transformedY.push_back( m_polygonX[i]*std::sin(bm) + m_polygonY[i]*std::cos(bm) + y );
  }

  m_timeToDie = SDL_GetTicks() + gameWorld.laser.lifespan;
}

laser::~laser() {
  if ( gameWorld.debug ) {
    std::cout << "delete laser" << std::endl;
  }
}

int laser::degree() {
  return m_force.degree();
}

void laser::render( SDL_Renderer *renderer ) {
  float xAdd = 0;
  float yAdd = 0;

  m_force.calcOffsets(xAdd,yAdd);

  int oldX = m_x;
  int oldY = m_y;

  m_x += xAdd;
  m_y += yAdd;

  gameWorld.adjustBoundaries( m_x,m_y );

  for ( int i=0 ; i<m_polygonX.size() ; i++ ) {
    m_transformedX[i] = m_transformedX[i] - oldX + m_x;
    m_transformedY[i] = m_transformedY[i] - oldY + m_y;
  }

  drawObjectWithMirrors( renderer,m_transformedX,m_transformedY );
}

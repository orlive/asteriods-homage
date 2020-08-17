#include "tools.h"
#include "laser.h"

laser::laser( float x,float y,float degree ) {
  addToPolygon( { 0,-gameWorld.ship.size/2 } ); 
  addToPolygon( { 0,-gameWorld.ship.size/2 - gameWorld.laser.size } );

  m_position.x = x;
  m_position.y = y;
  m_force.add(degree,gameWorld.laser.speed);

  float bm = bogenmass(degree);

  for ( std::size_t i=0 ; i<m_polygon.size() ; i++ ) {
    m_transformed.push_back( { m_polygon[i].x*std::cos(bm) - m_polygon[i].y*std::sin(bm) + x ,
                               m_polygon[i].x*std::sin(bm) + m_polygon[i].y*std::cos(bm) + y } );
  }

  m_timeToDie = gameWorld.time.lastTicks + gameWorld.laser.lifespan;
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
  POINT add = { 0.0f , 0.0f };

  m_force.calcOffsets( add );

  POINT oldPosition = m_position;
  
  m_position.x += add.x;
  m_position.y += add.y;

  gameWorld.adjustBoundaries( m_position );

  for ( std::size_t i=0 ; i<m_polygon.size() ; i++ ) {
    m_transformed[i].x = m_transformed[i].x - oldPosition.x + m_position.x;
    m_transformed[i].y = m_transformed[i].y - oldPosition.y + m_position.y;
  }

  drawObjectWithMirrors( renderer,m_transformed );
}

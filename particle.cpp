#include "tools.h"
#include "particle.h"

particle::particle( int x,int y,float randomPositionOffset,float speedStart,float speedEnd,float degreeStart,float degreeEnd ) {
  m_position.x = x + randomf( -randomPositionOffset/2,randomPositionOffset/2 );
  m_position.y = y + randomf( -randomPositionOffset/2,randomPositionOffset/2 );

  float r = randomf(degreeStart,degreeEnd);

  m_force.add( randomf(degreeStart,degreeEnd),random(speedStart,speedEnd) );
  m_timeToDie = gameWorld.time.lastTicks + gameWorld.particle.lifespan + random(0,gameWorld.particle.lifespan/10);
}

particle::~particle() {
  if ( gameWorld.debug ) {
    std::cout << "delete particle" << std::endl;
  }
}

void particle::move() {
  POINT add = { 0.0f , 0.0f };

  m_force.calcOffsets( add );

  m_position.x += add.x;
  m_position.y += add.y;

  gameWorld.adjustBoundaries( m_position );
}

void particle::render( SDL_Renderer *renderer ) {
  SDL_SetRenderDrawColor(renderer, 255,255,255, SDL_ALPHA_OPAQUE);
  base::drawPointWithMirror(renderer,m_position.x,m_position.y);
}    

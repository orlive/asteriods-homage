#include "tools.h"
#include "particle.h"

particle::particle( int x,int y,float randomPositionOffset,float speedStart,float speedEnd,float degreeStart,float degreeEnd ) {
  m_x = x + randomf( -randomPositionOffset/2,randomPositionOffset/2 );
  m_y = y + randomf( -randomPositionOffset/2,randomPositionOffset/2 );

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
  float xAdd = 0;
  float yAdd = 0;

  m_force.calcOffsets(xAdd,yAdd);

  m_x += xAdd;
  m_y += yAdd;

  gameWorld.adjustBoundaries( m_x,m_y );
}

void particle::render( SDL_Renderer *renderer ) {
  SDL_SetRenderDrawColor(renderer, 255,255,255, SDL_ALPHA_OPAQUE);
  base::drawPointWithMirror(renderer,m_x,m_y);
}    

#include <cmath>
#include <vector>
#include "config.h"
#include "tools.h"
#include "force.h"

force::force( float magnitude , float randomPercentage , int degree ) {
  if ( randomPercentage ) {
    magnitude += random( 0-magnitude/2,magnitude/2 ) * randomPercentage / 100;
  }
  this->add( degree?degree:random(0,359) , magnitude );
}

force::~force() {
  if ( config.debug ) {
    std::cout << "delete force" << std::endl;
  }
}

void force::calcCombinedOffsets( POINT& add ) {
  calcOffsets( add );
  m_combined.x += add.x;
  m_combined.y += add.y;
  add.x = m_combined.x;
  add.y = m_combined.y;
}

void force::calcOffsets( POINT& add ) {
  add.x = config.time.timeFactor * m_position.x;
  add.y = config.time.timeFactor * m_position.y;
}

void force::slowDown() {
  float deg  = degree();
  float size = magnitude();

  size -= ( config.time.timeFactor * size * config.ship.resistance );

  if ( size < 0.05f ) {
    stop();
  } else {
    float bm = bogenmass(deg);
    setPosition( { + size * std::sin(bm) , - size * std::cos(bm) } );
  }
}

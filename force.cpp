#include <cmath>
#include <vector>
#include "gameWorld.h"
#include "tools.h"
#include "force.h"

force::force( float magnitude , float randomPercentage , int degree ) {
  if ( randomPercentage ) {
    magnitude += random( 0-magnitude/2,magnitude/2 ) * randomPercentage / 100;
  }
  this->add( degree?degree:random(0,359) , magnitude );
}

force::~force() {
  if ( gameWorld.debug ) {
    std::cout << "delete force" << std::endl;
  }
}

void force::calcCombinedOffsets( float& xAdd,float& yAdd ) {
  calcOffsets(xAdd,yAdd);
  m_combinedX += xAdd;
  m_combinedY += yAdd;
  xAdd = m_combinedX;
  yAdd = m_combinedY;
}

void force::calcOffsets( float& xAdd,float& yAdd ) {
  xAdd = gameWorld.time.timeFactor * m_x;
  yAdd = gameWorld.time.timeFactor * m_y;
}

void force::slowDown() {
  float deg  = degree();
  float size = magnitude();

  size -= ( gameWorld.time.timeFactor * size * gameWorld.ship.resistance );

  if ( size < 0.05f ) {
    stop();
  } else {
    float bm = bogenmass(deg);
    setXY( + size * std::sin(bm) , - size * std::cos(bm) );
  }
}

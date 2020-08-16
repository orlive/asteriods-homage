#include <cmath>
#include "vector.h"
#include "tools.h"

const double PI = std::acos(-1);

vector::vector( float x , float y ) {
  m_x = x;
  m_y = y;
}

void vector::stop() {
  setXY( 0.0f , 0.0f );
}

void vector::setXY( float x, float y ) {
  m_x = x;
  m_y = y;
}

void vector::add( float angle,float magnitude ) {
  float bm = bogenmass(angle);
  m_x += magnitude * std::sin(bm);
  m_y -= magnitude * std::cos(bm);
}

float vector::rad() {
  float rad = std::atan( m_y / m_x );

  if ( m_x < 0 ) return rad + PI;
  if ( m_y < 0 ) return rad + PI * 2;

  return rad;
}

float vector::degree() {
  float deg = grad( rad() ) + 90; 
  while (deg>360.0f) deg-=360.0f;

  return deg;
}

float vector::magnitude() {
  return std::sqrtf( m_x * m_x + m_y * m_y );
}

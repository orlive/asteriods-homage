#include <cmath>
#include "vector.h"
#include "tools.h"

const double PI = std::acos(-1);

vector::vector( POINT position ) {
  m_position = position;
}

void vector::stop() {
  setPosition( { 0.0f , 0.0f } );
}

void vector::setPosition( POINT position ) {
  m_position = position;
}

void vector::add( float angle,float magnitude ) {
  float bm = bogenmass(angle);
  m_position.x += magnitude * std::sin(bm);
  m_position.y -= magnitude * std::cos(bm);
}

float vector::rad() {
  float rad = std::atan( m_position.y / m_position.x );

  if ( m_position.x < 0 ) return rad + PI;
  if ( m_position.y < 0 ) return rad + PI * 2;

  return rad;
}

float vector::degree() {
  float deg = grad( rad() ) + 90; 
  while (deg>360.0f) deg-=360.0f;

  return deg;
}

float vector::magnitude() {
  return std::sqrtf( m_position.x * m_position.x + m_position.y * m_position.y );
}

#include "tools.h"
#include "rock.h"

rock::rock(int x,int y,float size,float speed,int degree) {
  addToPolygon(  -5,-10 );
  addToPolygon(   0, -5 );
  addToPolygon(   5,-10 );
  addToPolygon(  10, -6 );
  addToPolygon(   7,  0 );
  addToPolygon(  10,  5 );
  addToPolygon(   5, 10 );
  addToPolygon(  -6, 10 );
  addToPolygon( -10,  5 );
  addToPolygon( -10, -3 );

  m_x = x;
  m_y = y;
  m_size = size;
  m_timeToDie = gameWorld.time.lastTicks + gameWorld.rock.lifespan + random(0,gameWorld.rock.lifespan/10);

  m_force        = std::shared_ptr<force>( new force(speed,20,degree) );
  m_degree       = random(0,359);
  m_degreeOffset = randomf(-2.0f,2.0f);
}

rock::~rock() {
  if ( gameWorld.debug ) {
    std::cout << "delete rock" << std::endl;
  }
}

float rock::magnitude() {
  return m_force->magnitude();
}

float rock::degree() {
  return m_force->degree();
}

void rock::move() {
  float xAdd = 0;
  float yAdd = 0;

  if ( !gameWorld.rock.frezzed ) {
    m_force->calcOffsets(xAdd,yAdd);

    m_x += xAdd;
    m_y += yAdd;

    gameWorld.adjustBoundaries( m_x,m_y );
  }

  m_transformedX.clear();
  m_transformedY.clear();

  m_degree += m_degreeOffset;
  while ( m_degree < 0   ) m_degree += 360;
  while ( m_degree > 359 ) m_degree -= 360;
  float bm = bogenmass(m_degree);
  float d_x = std::sin(bm);
  float d_y = std::cos(bm);

  for ( int i=0 ; i<m_polygonX.size() ; i++ ) {
    m_transformedX.push_back( ( m_polygonX[i]*std::cos(bm) - m_polygonY[i]*std::sin(bm) ) * m_size + m_x );
    m_transformedY.push_back( ( m_polygonX[i]*std::sin(bm) + m_polygonY[i]*std::cos(bm) ) * m_size + m_y );
  }
}

void rock::render( SDL_Renderer *renderer ) {
  //SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  drawObjectWithMirrors( renderer,m_transformedX,m_transformedY );
}

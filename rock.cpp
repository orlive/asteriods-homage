#include "tools.h"
#include "rock.h"

rock::rock(int x,int y,float size,float speed,int degree) {
  addToPolygon( {  -5,-10 } );
  addToPolygon( {   0, -5 } );
  addToPolygon( {   5,-10 } );
  addToPolygon( {  10, -6 } );
  addToPolygon( {   7,  0 } );
  addToPolygon( {  10,  5 } );
  addToPolygon( {   5, 10 } );
  addToPolygon( {  -6, 10 } );
  addToPolygon( { -10,  5 } );
  addToPolygon( { -10, -3 } );

  m_position.x = x;
  m_position.y = y;
  m_size = size;
  // m_timeToDie = config.time.lastTicks + config.rock.lifespan + random(0,config.rock.lifespan/10);
  m_timeToDie = 0;

  m_force        = std::shared_ptr<force>( new force(speed,20,degree) );
  m_degree       = random(0,359);
  m_degreeOffset = randomf(-2.0f,2.0f);
}

rock::~rock() {
  if ( config.debug ) {
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
  POINT add = { 0.0f , 0.0f };

  if ( !config.rock.frezzed ) {
    m_force->calcOffsets( add );

    m_position.x += add.x;
    m_position.y += add.y;

    config.adjustBoundaries( m_position );
  }

  m_transformed.clear();

  m_degree += m_degreeOffset;
  while ( m_degree < 0   ) m_degree += 360;
  while ( m_degree > 359 ) m_degree -= 360;
  float bm = bogenmass(m_degree);

  for ( std::size_t i=0 ; i<m_polygon.size() ; i++ ) {
    m_transformed.push_back( { ( m_polygon[i].x*std::cos(bm) - m_polygon[i].y*std::sin(bm) ) * m_size + m_position.x ,
                               ( m_polygon[i].x*std::sin(bm) + m_polygon[i].y*std::cos(bm) ) * m_size + m_position.y } );
  }
}

void rock::render( SDL_Renderer *renderer ) {
  //SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  drawObjectWithMirrors( renderer,m_transformed );
}

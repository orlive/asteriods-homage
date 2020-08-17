#include "tools.h"
#include "base.h"

int  base::x()           { return m_position.x; }
int  base::y()           { return m_position.y; }
int  base::size()        { return m_size; }
bool base::isDestroyed() { return m_destroyed; }

std::vector<POINT>& base::transformed() { return m_transformed; }

bool base::deceased() {
  return ( m_timeToDie!=0 && gameWorld.time.lastTicks > m_timeToDie );
}

void base::clearPolygon() {
  m_polygon.clear();
}

void base::addToPolygon(POINT position) {
  m_polygon.push_back(position);
}

void base::generatePosWidthMinDistance( int& x , int& y , int distance ) {
  for ( int i=0 ; i<100 ; i++ ) { // max. 100 tries
    x = random(0,gameWorld.display.width);
    y = random(0,gameWorld.display.height);

    int diffX = m_position.x - x;
    int diffY = m_position.y - y;

    if ( std::sqrt( diffX*diffX + diffY*diffY ) >= distance ) {
      return;
    }
  }
}

void base::drawObject( SDL_Renderer *renderer , std::vector<POINT>& xy , const int offsetX , const int offsetY ) {
  int   s = xy.size();
  POINT destroyed = { 0.0f , 0.0f };
  for ( int i=0 ; i<s-1 ; i++ ) {
    if ( m_destroyed ) {
      m_destroyedForces.at(i)->calcCombinedOffsets( destroyed );
      m_destroyedForces.at(i)->slowDown();
    }
    if ( gameWorld.display.bold ) {
      SDL_SetRenderDrawColor(renderer, 125,125,125, 100);
      SDL_RenderDrawLine(renderer , xy[i].x+offsetX+destroyed.x   , xy[i].y+offsetY+destroyed.y+1 , xy[i+1].x+offsetX+destroyed.x   , xy[i+1].y+offsetY+destroyed.y+1 );
      SDL_RenderDrawLine(renderer , xy[i].x+offsetX+destroyed.x+1 , xy[i].y+offsetY+destroyed.y+1 , xy[i+1].x+offsetX+destroyed.x+1 , xy[i+1].y+offsetY+destroyed.y+1 );
      SDL_RenderDrawLine(renderer , xy[i].x+offsetX+destroyed.x+1 , xy[i].y+offsetY+destroyed.y   , xy[i+1].x+offsetX+destroyed.x+1 , xy[i+1].y+offsetY+destroyed.y   );
    }
    SDL_SetRenderDrawColor(renderer, 255,255,255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer , xy[i].x+offsetX+destroyed.x   , xy[i].y+offsetY+destroyed.y   , xy[i+1].x+offsetX+destroyed.x   , xy[i+1].y+offsetY+destroyed.y   );
  }
  if ( m_destroyed ) {
    m_destroyedForces.at(s-1)->calcCombinedOffsets( destroyed );
    m_destroyedForces.at(s-1)->slowDown();
  }
  if ( gameWorld.display.bold ) {
    SDL_SetRenderDrawColor(renderer, 125,125,125, 100);
    SDL_RenderDrawLine(renderer , xy[s-1].x+offsetX+destroyed.x   , xy[s-1].y+offsetY+destroyed.y+1 , xy[0].x+offsetX+destroyed.x   , xy[0].y+offsetY+destroyed.y+1 );
    SDL_RenderDrawLine(renderer , xy[s-1].x+offsetX+destroyed.x+1 , xy[s-1].y+offsetY+destroyed.y+1 , xy[0].x+offsetX+destroyed.x+1 , xy[0].y+offsetY+destroyed.y+1 );
    SDL_RenderDrawLine(renderer , xy[s-1].x+offsetX+destroyed.x+1 , xy[s-1].y+offsetY+destroyed.y   , xy[0].x+offsetX+destroyed.x+1 , xy[0].y+offsetY+destroyed.y   );
  }
  SDL_SetRenderDrawColor(renderer, 255,255,255, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(renderer , xy[s-1].x+offsetX+destroyed.x    , xy[s-1].y+offsetY+destroyed.y    , xy[0].x+offsetX+destroyed.x    , xy[0].y+offsetY+destroyed.y    );
}

void base::drawObjectWithMirrors( SDL_Renderer *renderer , std::vector<POINT>& xy ) {
  if ( gameWorld.display.mirror ) {
    for ( int mX = -1 ; mX <= 1 ; mX++ ) {
      for ( int mY = -1 ; mY <= 1 ; mY++ ) {
        drawObject( renderer,xy , gameWorld.display.width * mX , gameWorld.display.height * mY );
      }
    }
  } else {
    drawObject( renderer , xy , 0 , 0 );
  }
}

void base::drawPoint( SDL_Renderer *renderer , int x , int y , int offsetX , int offsetY ) {
  SDL_RenderDrawPoint( renderer , x+offsetX   , y+offsetY   );
  if ( gameWorld.display.bold ) {
    SDL_RenderDrawPoint( renderer , x+offsetX+1 , y+offsetY   );
    SDL_RenderDrawPoint( renderer , x+offsetX   , y+offsetY+1 );
    SDL_RenderDrawPoint( renderer , x+offsetX+1 , y+offsetY+1 );
  }
}

void base::drawPointWithMirror( SDL_Renderer *renderer , int x , int y ) {
  if ( gameWorld.display.mirror ) {
    for ( int mX = -1 ; mX <= 1 ; mX++ ) {
      for ( int mY = -1 ; mY <= 1 ; mY++ ) {
        drawPoint( renderer , x , y , gameWorld.display.width * mX , gameWorld.display.height * mY );
      }
    }
  } else {
    drawPoint( renderer , x , y , 0 , 0 );
  }
}

#define SWAP(a,b) { float tmp=a;a=b;b=tmp; }

int base::KreuzProdTest(float x_A,float y_A,float x_B,float y_B,float x_C,float y_C) {
  if ( y_B > y_C ) {
    SWAP(x_B,x_C);
    SWAP(y_B,y_C);
  }
  if ( y_A <= y_B || y_A > y_C ) {
    return 1;
  }
  float Delta = (x_B - x_A) * (y_C - y_A) - (y_B - y_A) * (x_C - x_A);
  if ( Delta > 0 ) return 1;
  if ( Delta < 0 ) return -1;
  return 0;
}

bool base::pointInPolygon( POINT position,std::vector<POINT>& xy , int offsetX , int offsetY ) {
  int polygonSize = xy.size();
  int t = -1;
  for ( int i=0 ; i<polygonSize-1 ; i++ ) {
    t = t * KreuzProdTest( position.x,position.y,xy[i].x+offsetX,xy[i].y+offsetY,xy[i+1].x+offsetX,xy[i+1].y+offsetY );
    if ( t==0 ) {
      break;
    }
  }
  if ( polygonSize>1 ) { // polygon schliessen....
    t = t * KreuzProdTest( position.x,position.y,xy[polygonSize-1].x+offsetX,xy[polygonSize-1].y+offsetY,xy[0].x+offsetX,xy[0].y+offsetY );
  }  
  return (t>=0); // innerhalb oder des Polygons oder auf einer Linie
}

bool base::collision( std::vector<POINT>& polygon2 ) {
  if ( gameWorld.display.mirror ) {
    for ( int mX=-1 ; mX<=1 ; mX++ ) {
      for ( int mY=-1 ; mY<=1 ; mY++ ) {
        for( std::size_t i=0 ; i<m_transformed.size() ; i++ ) {
          if ( pointInPolygon( m_transformed[i] , polygon2 , gameWorld.display.width * mX,gameWorld.display.height * mY ) ) {
            return true;
          }
        }
      }
    }
  } else {
    for( std::size_t i=0 ; i<m_transformed.size() ; i++ ) {
      if ( pointInPolygon( m_transformed[i] , polygon2 , 0,0 ) ) {
        return true;
      }
    }
  }
  return false;
}

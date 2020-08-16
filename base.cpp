#include "tools.h"
#include "base.h"

int  base::x()           { return m_x; }
int  base::y()           { return m_y; }
int  base::size()        { return m_size; }
bool base::isDestroyed() { return m_destroyed; }

std::vector<int>& base::transedX() { return m_transformedX; }
std::vector<int>& base::transedY() { return m_transformedY; }

bool base::deceased() {
  return ( m_timeToDie==0 || gameWorld.time.lastTicks > m_timeToDie );
}

void base::clearPolygon() {
  m_polygonX.clear();
  m_polygonY.clear();
}

void base::addToPolygon(int x,int y) {
  m_polygonX.push_back(x);
  m_polygonY.push_back(y);
}

void base::generatePosWidthMinDistance( int& x , int& y , int distance ) {
  for ( int i=0 ; i<100 ; i++ ) { // max. 100 tries
    x = random(0,gameWorld.display.width);
    y = random(0,gameWorld.display.height);

    int diffX = m_x - x;
    int diffY = m_y - y;

    if ( std::sqrt( diffX*diffX + diffY*diffY ) >= distance ) {
      return;
    }
  }
}

void base::drawObject( SDL_Renderer *renderer , std::vector<int>& xn , std::vector<int>& yn , const int offsetX , const int offsetY ) {
  int   s = xn.size();
  float xDestroyed = 0;
  float yDestroyed = 0;
  for ( int i=0 ; i<s-1 ; i++ ) {
    if ( m_destroyed ) {
      m_destroyedForces.at(i)->calcCombinedOffsets(xDestroyed,yDestroyed);
      m_destroyedForces.at(i)->slowDown();
    }
    if ( gameWorld.display.bold ) {
      SDL_SetRenderDrawColor(renderer, 125,125,125, 100);
      SDL_RenderDrawLine(renderer , xn[i]+offsetX+xDestroyed   , yn[i]+offsetY+yDestroyed+1 , xn[i+1]+offsetX+xDestroyed   , yn[i+1]+offsetY+yDestroyed+1 );
      SDL_RenderDrawLine(renderer , xn[i]+offsetX+xDestroyed+1 , yn[i]+offsetY+yDestroyed+1 , xn[i+1]+offsetX+xDestroyed+1 , yn[i+1]+offsetY+yDestroyed+1 );
      SDL_RenderDrawLine(renderer , xn[i]+offsetX+xDestroyed+1 , yn[i]+offsetY+yDestroyed   , xn[i+1]+offsetX+xDestroyed+1 , yn[i+1]+offsetY+yDestroyed   );
    }
    SDL_SetRenderDrawColor(renderer, 255,255,255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer , xn[i]+offsetX+xDestroyed   , yn[i]+offsetY+yDestroyed   , xn[i+1]+offsetX+xDestroyed   , yn[i+1]+offsetY+yDestroyed   );
  }
  if ( m_destroyed ) {
    m_destroyedForces.at(s-1)->calcCombinedOffsets(xDestroyed,yDestroyed);
    m_destroyedForces.at(s-1)->slowDown();
  }
  if ( gameWorld.display.bold ) {
    SDL_SetRenderDrawColor(renderer, 125,125,125, 100);
    SDL_RenderDrawLine(renderer , xn[s-1]+offsetX+xDestroyed   , yn[s-1]+offsetY+yDestroyed+1 , xn[0]+offsetX+xDestroyed   , yn[0]+offsetY+yDestroyed+1 );
    SDL_RenderDrawLine(renderer , xn[s-1]+offsetX+xDestroyed+1 , yn[s-1]+offsetY+yDestroyed+1 , xn[0]+offsetX+xDestroyed+1 , yn[0]+offsetY+yDestroyed+1 );
    SDL_RenderDrawLine(renderer , xn[s-1]+offsetX+xDestroyed+1 , yn[s-1]+offsetY+yDestroyed   , xn[0]+offsetX+xDestroyed+1 , yn[0]+offsetY+yDestroyed   );
  }
  SDL_SetRenderDrawColor(renderer, 255,255,255, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(renderer , xn[s-1]+offsetX+xDestroyed    , yn[s-1]+offsetY+yDestroyed    , xn[0]+offsetX+xDestroyed    , yn[0]+offsetY+yDestroyed    );
}

void base::drawObjectWithMirrors( SDL_Renderer *renderer , std::vector<int>& xn , std::vector<int>& yn ) {
  if ( gameWorld.display.mirror ) {
    for ( int mX = -1 ; mX <= 1 ; mX++ ) {
      for ( int mY = -1 ; mY <= 1 ; mY++ ) {
        drawObject( renderer,xn,yn , gameWorld.display.width * mX , gameWorld.display.height * mY );
      }
    }
  } else {
    drawObject( renderer , xn , yn , 0 , 0 );
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

bool base::pointInPolygon( float x,float y,std::vector<int>& xn , std::vector<int>& yn , int offsetX , int offsetY ) {
  int polygonSize = xn.size();
  int t = -1;
  for ( int i=0 ; i<polygonSize-1 ; i++ ) {
    t = t * KreuzProdTest( x,y,xn[i]+offsetX,yn[i]+offsetY,xn[i+1]+offsetX,yn[i+1]+offsetY );
    if ( t==0 ) {
      break;
    }
  }
  if ( polygonSize>1 ) { // polygon schliessen....
    t = t * KreuzProdTest( x,y,xn[polygonSize-1]+offsetX,yn[polygonSize-1]+offsetY,xn[0]+offsetX,yn[0]+offsetY );
  }  
  return (t>=0); // innerhalb oder des Polygons oder auf einer Linie
}

bool base::collision( std::vector<int>& polygon2_X,std::vector<int>& polygon2_Y ) {
  if ( gameWorld.display.mirror ) {
    for ( int mX=-1 ; mX<=1 ; mX++ ) {
      for ( int mY=-1 ; mY<=1 ; mY++ ) {
        for( int i=0 ; i<m_transformedX.size() ; i++ ) {
          if ( pointInPolygon( m_transformedX[i],m_transformedY[i] , polygon2_X,polygon2_Y , gameWorld.display.width  * mX,gameWorld.display.height * mY ) ) {
            return true;
          }
        }
      }
    }
  } else {
    for( int i=0 ; i<m_transformedX.size() ; i++ ) {
      if ( pointInPolygon( m_transformedX[i],m_transformedY[i] , polygon2_X,polygon2_Y , 0,0 ) ) {
        return true;
      }
    }
  }
  return false;
}

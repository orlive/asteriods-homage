#ifndef BASE_DEFINE
#define BASE_DEFINE

#include "force.h"
#include "sdlEngine.h"
#include "gameWorld.h"

class base {
  protected:
    std::vector<POINT> m_polygon;
    std::vector<POINT> m_transformed;

    std::vector< std::shared_ptr<force> > m_destroyedForces;

    int   m_timeToDie;
    bool  m_destroyed = false;
    float m_size = 0;

    POINT m_position;

  public:
    int  x();
    int  y();
    int  size();
    bool isDestroyed();

    std::vector<POINT>& transformed();

    bool deceased();
    void clearPolygon();
    void addToPolygon(POINT position);
    void generatePosWidthMinDistance( int& x , int& y , int distance=0 );
    void drawObject( SDL_Renderer *renderer , std::vector<POINT>& xy , const int offsetX , const int offsetY );
    void drawObjectWithMirrors( SDL_Renderer *renderer , std::vector<POINT>& xy );
    void drawPoint( SDL_Renderer *renderer , int x , int y , int offsetX , int offsetY );
    void drawPointWithMirror( SDL_Renderer *renderer , int x , int y );
    int KreuzProdTest(float x_A,float y_A,float x_B,float y_B,float x_C,float y_C);
    bool pointInPolygon( POINT position , std::vector<POINT>& xy , int offsetX , int offsetY );
    bool collision( std::vector<POINT>& polygon2 );
};

#endif

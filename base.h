#ifndef BASE_DEFINE
#define BASE_DEFINE

#include "force.h"
#include "sdlEngine.h"
#include "gameWorld.h"

class base {
  protected:
    std::vector<int> m_polygonX;
    std::vector<int> m_polygonY;
    std::vector<int> m_transformedX;
    std::vector<int> m_transformedY;

    std::vector< std::shared_ptr<force> > m_destroyedForces;

    int   m_timeToDie;
    bool  m_destroyed = false;
    float m_size = 0;
    float m_x = 0;
    float m_y = 0;

  public:
    int  x();
    int  y();
    int  size();
    bool isDestroyed();

    std::vector<int>& transedX();
    std::vector<int>& transedY();

    bool deceased();
    void clearPolygon();
    void addToPolygon(int x,int y);
    void generatePosWidthMinDistance( int& x , int& y , int distance=0 );
    void drawObject( SDL_Renderer *renderer , std::vector<int>& xn , std::vector<int>& yn , const int offsetX , const int offsetY );
    void drawObjectWithMirrors( SDL_Renderer *renderer , std::vector<int>& xn , std::vector<int>& yn );
    void drawPoint( SDL_Renderer *renderer , int x , int y , int offsetX , int offsetY );
    void drawPointWithMirror( SDL_Renderer *renderer , int x , int y );
    int KreuzProdTest(float x_A,float y_A,float x_B,float y_B,float x_C,float y_C);
    bool pointInPolygon( float x,float y,std::vector<int>& xn , std::vector<int>& yn , int offsetX , int offsetY );
    bool collision( std::vector<int>& polygon2_X,std::vector<int>& polygon2_Y );
};

#endif

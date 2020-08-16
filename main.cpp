#include <SDL.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <SDL_ttf.h>

#define D std::cout << __LINE__ << std::endl;
const double PI = std::acos(-1);

#define BOGENMASS(a) ( ( PI / 180.0f ) * (float)a )
#define GRAD(a)      ( (float)a / ( PI / 180 ) )
#define CHARWIDTH 17
#define CHARHEIGHT 35

float randomf(float low, float high) {
  if (low > high)
    return high;
  return low + (std::rand() % (int)(high - low + 1));
}

int random(int low, int high) {
  if (low > high)
    return high;
  return low + (std::rand() % (high - low + 1));
}

struct /* GAMEWORLD */ {

  struct /* time */ {
    int   lastTicks;
    int   elapsed;
    float timeFactor;
  } time;

  struct /* display */ {
    int  width  = 800;
    int  height = 640;
    bool bold   = true;
    bool mirror = true;
  } display;

  struct /*ship */ { 
    int   size       = 25;
    float resistance = 0.5f;
    int   laser_wait = 200;

    struct /* thrust_particles */ {
      int min_speed  = 100;
      int max_speed  = 200;
      int min_degree = -180-20;
      int max_degree = -180+20;
    } thrust_particles;
  } ship;

  struct /* laser */ {
    int lifespan = 1000;
    int speed    = 400;
    int size     = 20;
  } laser;

  struct /* particle */ {
    int lifespan = 500;
  } particle;

  struct /* rock */ {
    int  lifespan = 100000;
    bool frezzed;  // for debugging purpose
  } rock;

  bool  debug      = false;
  int   game_delay = 10;

  void toggleRockFreezed() {
    static int nextToogleRockFreezed;
    if ( time.lastTicks > nextToogleRockFreezed ) {
      nextToogleRockFreezed = time.lastTicks + 500;
      rock.frezzed = ! rock.frezzed;
    }
  }

  void toggleMirrorMode() {
    static int nextToogleMirrorMode;
    if ( time.lastTicks > nextToogleMirrorMode ) {
      nextToogleMirrorMode = time.lastTicks + 500;
      display.mirror = ! display.mirror;
    }
  }

  void toggleBoldMode() {
    static int nextToogleBoldMode;
    if ( time.lastTicks > nextToogleBoldMode ) {
      nextToogleBoldMode = time.lastTicks + 500;
      display.bold = ! display.bold;
    }
  }
  
  void adjustBoundaries( int& x , int& y ) {
    while ( x > display.width  ) { x-=display.width;  };
    while ( x < 0              ) { x+=display.width;  };
    while ( y > display.height ) { y-=display.height; };
    while ( y < 0              ) { y+=display.height; };
  }

  void adjustBoundaries( float& x , float& y ) {
    while ( x > display.width  ) { x-=display.width;  };
    while ( x < 0              ) { x+=display.width;  };
    while ( y > display.height ) { y-=display.height; };
    while ( y < 0              ) { y+=display.height; };
  }

  void processArguments( int argc , char* argv[] ) {
    for ( int i=1 ; i<argc ; i++ ) {
      char*p = argv[i];
      if ( *p!='-' ) continue;
      if ( *(++p)=='-' ) p++;
      switch( *p ) {
        case 'd' :
        case 'v' : debug          = true; break;
        case 'b' : display.bold   = false; break;
        case 'm' : display.mirror = false; break;
      }
    }
  }

} gameWorld;

class sdlEngine {
  private:
    SDL_Window   *m_window;
    SDL_Renderer *m_renderer;
    SDL_Cursor   *m_cursor;
    TTF_Font     *m_font;
    SDL_Color     m_fontColor = {255, 255, 255, 255};
    SDL_Color     m_backColor = {  0,   0,   0, 255};

    const Uint8  *m_state;

  public:
    // TODO -> throw errors!!!

    sdlEngine( const char *title ) {
      if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",SDL_GetError());
        return;
      }

      if ( ( m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND) ) == NULL ) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Get the system hand cursor: %s", SDL_GetError());
        return;
      }
      SDL_SetCursor(m_cursor);

      if (SDL_CreateWindowAndRenderer(gameWorld.display.width, gameWorld.display.height, 0, &m_window, &m_renderer) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Create window and renderer: %s", SDL_GetError());
        return;
      }

      SDL_SetWindowTitle( m_window , title );
      if ( TTF_Init() < 0 ) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL_ttf: %s",TTF_GetError());
        return;
      }
        
      m_font = TTF_OpenFont("assets/FiraMono-Regular.ttf", 20);
      if (m_font == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Open font: %s",TTF_GetError());
        return;
      }
    }

    ~sdlEngine() {
      TTF_CloseFont(m_font);
      SDL_DestroyRenderer(m_renderer);
      SDL_DestroyWindow(m_window);
      SDL_Quit();
    }

    void newLoop() {
      gameWorld.time.elapsed    = SDL_GetTicks() - gameWorld.time.lastTicks;
      gameWorld.time.lastTicks  = SDL_GetTicks();
      gameWorld.time.timeFactor = (float)gameWorld.time.elapsed / 1000.0f;
    }

    const Uint8* state() {
      return m_state;
    }

    SDL_Renderer* renderer() {
      return m_renderer;
    }

    bool haveQuitEvent() {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if ( event.type==SDL_QUIT || ( event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_q ) ) {
          return true;
        }
      }
      return false;
    }
  
    void newInput() {
      SDL_PumpEvents();
      this->m_state = SDL_GetKeyboardState(NULL);
    }

    void use2screen() {
      printf("SDL_GetNumVideoDisplays() : %d\n",SDL_GetNumVideoDisplays());
      if ( SDL_GetNumVideoDisplays() > 1 ) {
        int x,y;
        SDL_GetWindowPosition( m_window,&x,&y);
        SDL_SetWindowPosition( m_window,x+1980,y);
        printf("x/y:%d/%d\n",x,y);
      }
    }

    SDL_Texture* genTexture( const char* text ) {
      SDL_Surface *surface = TTF_RenderText_Blended( m_font,text,m_fontColor );
      SDL_Texture *texture = SDL_CreateTextureFromSurface(m_renderer, surface);
      SDL_FreeSurface(surface);

      return texture;
    }

    void drawTexture( int x,int y,int w,int h,SDL_Texture* texture ) {
      if ( texture ) {
        SDL_Rect dest = { .x = x , .y = y , .w = w , .h = h };
        SDL_RenderCopy(m_renderer, texture, NULL, &dest);
      }
    }

    void drawText( int x,int y,int w,int h,const char* text ) {
      SDL_Texture *texture = genTexture( text );
      drawTexture( x,y,w,h,texture );
      SDL_DestroyTexture(texture);
    }
};

class vector {
  protected:
    float m_x = 0;
    float m_y = 0;

  public:
    vector( float x=0 , float y=0 ) {
      m_x = x;
      m_y = y;
    }

    void stop() {
      setXY( 0.0f , 0.0f );
    }

    void setXY( float x, float y ) {
      m_x = x;
      m_y = y;
    }

    void add( float angle,float magnitude ) {
      float bogenmass = BOGENMASS(angle);
      m_x += magnitude * std::sin(bogenmass);
      m_y -= magnitude * std::cos(bogenmass);
    }

    float rad() {
      float rad = std::atan( m_y / m_x );

      if ( m_x < 0 ) return rad + PI;
      if ( m_y < 0 ) return rad + PI * 2;

      return rad;
    }

    float degree() {
      float deg = GRAD( rad() ) + 90; 
      while (deg>360.0f) deg-=360.0f;

      return deg;
    }

    float magnitude() {
      return std::sqrtf( m_x * m_x + m_y * m_y );
    }
};

class force : public vector {
  private:
    float m_combinedX = 0.0f;
    float m_combinedY = 0.0f;

  public:
    force( float magnitude=0 , float randomPercentage=0 , int degree=0 ) {
      if ( randomPercentage ) {
        magnitude += random( 0-magnitude/2,magnitude/2 ) * randomPercentage / 100;
      }
      this->add( degree?degree:random(0,359) , magnitude );
    }

    ~force() {
      if ( gameWorld.debug ) {
        std::cout << "delete force" << std::endl;
      }
    }

    void calcCombinedOffsets( float& xAdd,float& yAdd ) {
      calcOffsets(xAdd,yAdd);
      m_combinedX += xAdd;
      m_combinedY += yAdd;
      xAdd = m_combinedX;
      yAdd = m_combinedY;
    }

    void calcOffsets( float& xAdd,float& yAdd ) {
      xAdd = gameWorld.time.timeFactor * m_x;
      yAdd = gameWorld.time.timeFactor * m_y;
    }

    void slowDown() {
      float deg  = degree();
      float size = magnitude();

      size -= ( gameWorld.time.timeFactor * size * gameWorld.ship.resistance );

      if ( size < 0.05f ) {
        stop();
      } else {
        float bogenmass = BOGENMASS(deg);
        setXY( + size * std::sin(bogenmass) , - size * std::cos(bogenmass) );
      }
    }
};

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
    int  x()           { return m_x; }
    int  y()           { return m_y; }
    int  size()        { return m_size; }
    bool isDestroyed() { return m_destroyed; }

    std::vector<int>& transedX() { return m_transformedX; }
    std::vector<int>& transedY() { return m_transformedY; }

    bool deceased() {
      return ( m_timeToDie==0 || gameWorld.time.lastTicks > m_timeToDie );
    }

    void clearPolygon() {
      m_polygonX.clear();
      m_polygonY.clear();
    }

    void addToPolygon(int x,int y) {
      m_polygonX.push_back(x);
      m_polygonY.push_back(y);
    }

    void generatePosWidthMinDistance( int& x , int& y , int distance=0 ) {
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

    void drawObject( SDL_Renderer *renderer , std::vector<int>& xn , std::vector<int>& yn , const int offsetX , const int offsetY ) {
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

    void drawObjectWithMirrors( SDL_Renderer *renderer , std::vector<int>& xn , std::vector<int>& yn ) {
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

    void drawPoint( SDL_Renderer *renderer , int x , int y , int offsetX , int offsetY ) {
      SDL_RenderDrawPoint( renderer , x+offsetX   , y+offsetY   );
      if ( gameWorld.display.bold ) {
        SDL_RenderDrawPoint( renderer , x+offsetX+1 , y+offsetY   );
        SDL_RenderDrawPoint( renderer , x+offsetX   , y+offsetY+1 );
        SDL_RenderDrawPoint( renderer , x+offsetX+1 , y+offsetY+1 );
      }
    }

    void drawPointWithMirror( SDL_Renderer *renderer , int x , int y ) {
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

    int KreuzProdTest(float x_A,float y_A,float x_B,float y_B,float x_C,float y_C) {
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

    bool pointInPolygon( float x,float y,std::vector<int>& xn , std::vector<int>& yn , int offsetX , int offsetY ) {
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

    bool collision( std::vector<int>& polygon2_X,std::vector<int>& polygon2_Y ) {
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
};

class rock : public base {
  private:
    std::shared_ptr<force> m_force;

  public:
    rock(int x,int y,float size,float speed,int degree=0) {
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

      m_force = std::shared_ptr<force>( new force(speed,20,degree) );
    }

    ~rock() {
      if ( gameWorld.debug ) {
        std::cout << "delete rock" << std::endl;
      }
    }

    float magnitude() {
      return m_force->magnitude();
    }

    float degree() {
      return m_force->degree();
    }

    void move() {
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

      for ( int i=0 ; i<m_polygonX.size() ; i++ ) {
        m_transformedX.push_back( m_polygonX[i] * m_size + m_x );
        m_transformedY.push_back( m_polygonY[i] * m_size + m_y );
      }
    }

    void render( SDL_Renderer *renderer ) {
      //SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      drawObjectWithMirrors( renderer,m_transformedX,m_transformedY );
    }
};

class laser : public base {
  private:
    force m_force;
    
  public:
    laser( float x,float y,float degree ) {
      addToPolygon(0,-gameWorld.ship.size/2); // TODO : use SHIP-SIZE
      addToPolygon(0,-gameWorld.ship.size/2 - gameWorld.laser.size);

      m_x = x;
      m_y = y;
      m_force.add(degree,gameWorld.laser.speed);

      float bogenmass = BOGENMASS(degree);

      for ( int i=0 ; i<m_polygonX.size() ; i++ ) {
        m_transformedX.push_back( m_polygonX[i]*std::cos(bogenmass) - m_polygonY[i]*std::sin(bogenmass) + x );
        m_transformedY.push_back( m_polygonX[i]*std::sin(bogenmass) + m_polygonY[i]*std::cos(bogenmass) + y );
      }

      m_timeToDie = SDL_GetTicks() + gameWorld.laser.lifespan;
    }

    ~laser() {
      if ( gameWorld.debug ) {
        std::cout << "delete laser" << std::endl;
      }
    }

    int degree() {
      return m_force.degree();
    }

    void render( SDL_Renderer *renderer ) {
      float xAdd = 0;
      float yAdd = 0;

      m_force.calcOffsets(xAdd,yAdd);

      int oldX = m_x;
      int oldY = m_y;

      m_x += xAdd;
      m_y += yAdd;

      gameWorld.adjustBoundaries( m_x,m_y );

      for ( int i=0 ; i<m_polygonX.size() ; i++ ) {
        m_transformedX[i] = m_transformedX[i] - oldX + m_x;
        m_transformedY[i] = m_transformedY[i] - oldY + m_y;
      }

      drawObjectWithMirrors( renderer,m_transformedX,m_transformedY );
    }
};
 
class particle : public base {
  private:
    force m_force;

  public:
    particle( int x,int y,float randomPositionOffset,float speedStart,float speedEnd,float degreeStart,float degreeEnd ) {
      m_x = x + randomf( -randomPositionOffset/2,randomPositionOffset/2 );
      m_y = y + randomf( -randomPositionOffset/2,randomPositionOffset/2 );

      float r = randomf(degreeStart,degreeEnd);

      m_force.add( randomf(degreeStart,degreeEnd),random(speedStart,speedEnd) );
      m_timeToDie = gameWorld.time.lastTicks + gameWorld.particle.lifespan + random(0,gameWorld.particle.lifespan/10);
    }

    ~particle() {
      if ( gameWorld.debug ) {
        std::cout << "delete particle" << std::endl;
      }
    }

    void move() {
      float xAdd = 0;
      float yAdd = 0;

      m_force.calcOffsets(xAdd,yAdd);

      m_x += xAdd;
      m_y += yAdd;

      gameWorld.adjustBoundaries( m_x,m_y );
    }

    void render( SDL_Renderer *renderer ) {
      SDL_SetRenderDrawColor(renderer, 255,255,255, SDL_ALPHA_OPAQUE);
      base::drawPointWithMirror(renderer,m_x,m_y);
    }    
};

class ship : public base {

  private:
    std::vector<int> m_thrustX;
    std::vector<int> m_thrustY;
    force m_force;
    float m_thrustOn = 0.0f;
    float m_degree;
    float m_speed;

  public:

    ship() {
      m_x    = 200.0f;
      m_y    = 200.0f;
      m_degree = 0;
      m_speed  = 0;

      resize(gameWorld.ship.size);
    }

    float getDegree() {
      return m_degree;
    }

    void renew() {
      if ( this->isDestroyed() ) {
        this->m_destroyed = false;
      }
    }

    void resize( int size ) {
      int offset = -(size/2);

      clearPolygon();

      addToPolygon(              0  , offset);
      addToPolygon(  size/5*2  , size+offset );
      addToPolygon(-(size/5*2) , size+offset );

      m_thrustX.clear();
      m_thrustY.clear();

      m_thrustX.push_back( size/8*2); m_thrustY.push_back( -offset*1.1f );
      m_thrustX.push_back(-size/8*2); m_thrustY.push_back( -offset*1.1f );
      m_thrustX.push_back(        0); m_thrustY.push_back( -offset*1.7f );
    }

    void thrust(std::vector< std::shared_ptr<particle> >& particles) {
      if ( m_destroyed ) return;

      m_force.add(m_degree,gameWorld.time.timeFactor * 200.0f);
      m_thrustOn = gameWorld.time.lastTicks + 200;

      float bogenmass = BOGENMASS(m_degree);
      int x = m_x - gameWorld.ship.size*std::sin(bogenmass);
      int y = m_y + gameWorld.ship.size*std::cos(bogenmass);

      particles.push_back( std::shared_ptr<particle>( new particle( x,y,gameWorld.ship.size,
                                                                    gameWorld.ship.thrust_particles.min_speed,
                                                                    gameWorld.ship.thrust_particles.max_speed,
                                                                    gameWorld.ship.thrust_particles.min_degree + m_degree,
                                                                    gameWorld.ship.thrust_particles.max_degree + m_degree ) ) );
    }

    void destroy(std::vector< std::shared_ptr<particle> >& particles) {
      if ( ! m_destroyed ) {
        m_destroyed = true;
        m_destroyedForces.clear();
        for ( int i=0 ; i<m_polygonX.size() ; i++ ) {
          m_destroyedForces.push_back( std::shared_ptr<force>( new force(20,10,random(0,359)) ) );
        }
        for ( int i=0 ; i<30 ; i++ ) {
          particles.push_back( std::shared_ptr<particle>( new particle( m_x,m_y,gameWorld.ship.size, /*gameWorld.ship.size*/
                                                                        5,         // speed min
                                                                        15,        // speed max
                                                                        0,         // degree min
                                                                        359 ) ) ); // degree max
        }
      }
    }

    void stop() {
      m_force.stop();
    }

    void setXY(float x=0,float y=0) {
      m_force.setXY(10*x,10*y);
    }

    void rotateLeft() {
      if ( m_destroyed ) return;
      m_degree -= gameWorld.time.timeFactor * 180;
      while ( m_degree < 0 ) m_degree += 360.0f;
    }

    void rotateRight() {
      if ( m_destroyed ) return;
      m_degree += gameWorld.time.timeFactor * 180;
      while ( m_degree >= 360 ) m_degree -= 360.0f;
    }

    void moveShip() {
      float xAdd = 0;
      float yAdd = 0;

      m_force.calcOffsets(xAdd,yAdd);

      m_x += xAdd;
      m_y += yAdd;

      gameWorld.adjustBoundaries( m_x,m_y );

      m_force.slowDown();
    }

    void drawShip( SDL_Renderer *renderer , std::vector<int>& xn , std::vector<int>& yn , const float offsetX , const float offsetY ) {
      // TODO -> fÃ¼r ein Array an Punkten kann auch eine Funktion in der Base-Class verwendet werden!
      SDL_RenderDrawLine(renderer , m_x+xn[0]+offsetX , m_y+yn[0]+offsetY , m_x+xn[1]+offsetX , m_y+yn[1]+offsetY);
      SDL_RenderDrawLine(renderer , m_x+xn[1]+offsetX , m_y+yn[1]+offsetY , m_x+xn[2]+offsetX , m_y+yn[2]+offsetY);
      SDL_RenderDrawLine(renderer , m_x+xn[2]+offsetX , m_y+yn[2]+offsetY , m_x+xn[0]+offsetX , m_y+yn[0]+offsetY);
    }

    void render( SDL_Renderer *renderer ) {
      SDL_SetRenderDrawColor(renderer, 255,255,255, SDL_ALPHA_OPAQUE);

      m_transformedX.clear();
      m_transformedY.clear();

      float bogenmass = BOGENMASS(m_degree);

      if ( this->isDestroyed() ) { 
        for ( int i=0 ; i<m_polygonX.size() ; i++ ) {
          float xAdd = 0;
          float yAdd = 0;

          m_destroyedForces.at(i)->calcOffsets(xAdd,yAdd);

          m_transformedX.push_back( m_polygonX[i]*std::cos(bogenmass) - m_polygonY[i]*std::sin(bogenmass) + m_x + xAdd );
          m_transformedY.push_back( m_polygonX[i]*std::sin(bogenmass) + m_polygonY[i]*std::cos(bogenmass) + m_y + yAdd );
        }
      } else {
        for ( int i=0 ; i<m_polygonX.size() ; i++ ) {
          m_transformedX.push_back( m_polygonX[i]*std::cos(bogenmass) - m_polygonY[i]*std::sin(bogenmass) + m_x );
          m_transformedY.push_back( m_polygonX[i]*std::sin(bogenmass) + m_polygonY[i]*std::cos(bogenmass) + m_y );
        }
      }

      drawObjectWithMirrors( renderer,m_transformedX,m_transformedY );

      if ( m_thrustOn > gameWorld.time.lastTicks ) {
        std::vector<int> xn;
        std::vector<int> yn;
        for ( int i=0 ; i<m_thrustX.size() ; i++ ) {
          int pointY = (i==m_thrustX.size()-1) ? random(2,17) : random(-1,1);
          int pointX = random(-1,1);
          xn.push_back( (pointX+m_thrustX[i])*std::cos(bogenmass) - ( pointY+m_thrustY[i])*std::sin(bogenmass) + m_x );
          yn.push_back( (pointX+m_thrustX[i])*std::sin(bogenmass) + ( pointY+m_thrustY[i])*std::cos(bogenmass) + m_y );
        }
        drawObjectWithMirrors( renderer,xn,yn );
      }
    }
};

struct /* LASERS */ {
  std::vector< std::shared_ptr<laser> > array;

  void newLaser(ship& aShip) {
    static int waitTill;
    if ( gameWorld.time.lastTicks > waitTill ) {
      array.push_back( std::shared_ptr<laser>( new laser(aShip.x(),aShip.y(),aShip.getDegree()) ) );
      waitTill = gameWorld.time.lastTicks + gameWorld.ship.laser_wait;
    }
  }

  void render( SDL_Renderer *renderer ) {
    for ( int i = 0 ; i < array.size() ; i++ ) {
      if ( array.at(i)->deceased() ) {
        array.erase(array.begin()+i);
        i--;
      } else {
        array.at(i)->render(renderer);
      }
    }
  }

} lasers;

struct /* routinen */ {
  typedef struct {
    SDL_Texture* texture;
    int          width;
  } UIText;

  UIText textLevel;
  UIText textScore;
  UIText textLives;
  UIText textFPS;

  int number2width(int n) {
    int w = 1;
    if ( n>9  ) w++;
    if ( n>99 ) w++;
    if ( n>999) w++;
    return w;
  }

  void genLevel( sdlEngine* engine,int level ) {
    char txt[20];
    if ( textLevel.texture ) SDL_DestroyTexture( textLevel.texture );
    sprintf( txt,"Level: %d",level );
    textLevel.texture = engine->genTexture( txt );
    textLevel.width   = number2width(level)+7;
  }

  void genScore( sdlEngine* engine,int score ) {
    char txt[20];
    if ( textScore.texture ) SDL_DestroyTexture( textScore.texture );
    sprintf( txt,"Score: %d",score );
    textScore.texture = engine->genTexture( txt );
    textScore.width   = number2width(score)+7;
  }

  void genLives( sdlEngine* engine,int lives ) {
    char txt[20];
    if ( textLives.texture ) SDL_DestroyTexture( textLives.texture );
    sprintf( txt,"Ships: %d",lives );
    textLives.texture = engine->genTexture( txt );
    textLives.width   = number2width(lives)+7;
  }

  void drawTexts( sdlEngine* engine ) {
    engine->drawTexture(   1,1,textLevel.width*CHARWIDTH,CHARHEIGHT,textLevel.texture );
    engine->drawTexture( 250,1,textScore.width*CHARWIDTH,CHARHEIGHT,textScore.texture );
    engine->drawTexture( 500,1,textLives.width*CHARWIDTH,CHARHEIGHT,textLives.texture );
  }

  void drawFPS( sdlEngine* engine ) {
    static int nextDraw;

    if ( gameWorld.time.lastTicks > nextDraw ) {
      nextDraw = gameWorld.time.lastTicks + 333;
      char txt[20];
      if ( textFPS.texture ) SDL_DestroyTexture( textFPS.texture );
      sprintf( txt,"(%03d)",(int)(1000.0f/(float)gameWorld.time.elapsed) );
      textFPS = { engine->genTexture( txt ) , 5 };
    }
    
    engine->drawTexture( 740,1,textFPS.width*CHARWIDTH/3*2,CHARHEIGHT/3*2,textFPS.texture );
  }

} routinen;

bool game( sdlEngine& gameSDL ) {
  ship aShip;
  std::vector< std::shared_ptr<rock> > rocks;
  std::vector< std::shared_ptr<particle> > particles;
  int level = 0;
  int score = 0;
  int lives = 3;

  routinen.genLevel( &gameSDL,level );
  routinen.genScore( &gameSDL,score );
  routinen.genLives( &gameSDL,lives );

  while ( true ) {
    gameSDL.newLoop();
    gameSDL.newInput();

    const Uint8* state = gameSDL.state();

    if ( state[SDL_SCANCODE_R]     ) if ( aShip.isDestroyed() ) return true;
    if ( state[SDL_SCANCODE_SPACE] ) lasers.newLaser(aShip);
    if ( state[SDL_SCANCODE_RIGHT] ) aShip.rotateRight();
    if ( state[SDL_SCANCODE_LEFT]  ) aShip.rotateLeft();
    if ( state[SDL_SCANCODE_UP]    ) aShip.thrust(particles);

    if ( state[SDL_SCANCODE_A]     ) aShip.renew(); // test

    if ( gameWorld.debug ) {
      if ( state[SDL_SCANCODE_0] ) aShip.stop(); // TODO - entfernen, wenn fertig
      if ( state[SDL_SCANCODE_F] ) gameWorld.toggleRockFreezed(); // TODO - entfernen, wenn fertig
      if ( state[SDL_SCANCODE_M] ) gameWorld.toggleMirrorMode(); // TODO - entfernen, wenn fertig
      if ( state[SDL_SCANCODE_B] ) gameWorld.toggleBoldMode(); // TODO - enfernen, wenn fertig
      if ( state[SDL_SCANCODE_8] ) aShip.resize( ++gameWorld.ship.size );
      if ( state[SDL_SCANCODE_9] ) aShip.resize( --gameWorld.ship.size );
      if ( state[SDL_SCANCODE_1] ) printf( "(%05ld) FPS:%f\n",rocks.size(),1000.0f/gameWorld.time.elapsed );
    }

    if ( gameSDL.haveQuitEvent() ) return false;

    SDL_SetRenderDrawColor(gameSDL.renderer(), 0,0,0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(gameSDL.renderer());

    routinen.drawTexts( &gameSDL );
    routinen.drawFPS( &gameSDL );

    aShip.moveShip( );
    aShip.render(gameSDL.renderer());

    lasers.render(gameSDL.renderer());

    // wenn keine Asteroiden mehr vorhanden sind... alte Anzahl verdoppeln und neu generieren
    if ( rocks.size() == 0 ) {
      routinen.genLevel( &gameSDL,++level );

      for ( int i=0 ; i<level*level ; i++ ) {
        int x,y;
        aShip.generatePosWidthMinDistance(x,y,200);
        rocks.push_back( std::shared_ptr<rock>( new rock(x,y,random(300,700)/100,100 ) ) );
      }
    }

    // für alle Asteroiden ...
    //   -> move
    //   -> Kollision mit dem Schiff?
    //   -> von einem Laser getroffen?
    //   -> darstellen

    for ( int i=0 ; i<rocks.size() ; i++ ) {
      int  hitRockIndex = -1;

      rocks[i]->move();

      // -> Kollision mit dem Schiff?
      if ( ! aShip.isDestroyed() ) {
        if ( aShip.collision( rocks[i]->transedX() ,rocks[i]->transedY() ) ) {
          aShip.destroy(particles);
          routinen.genLives( &gameSDL , --lives );
        }
      }

      // -> von einem Laser getroffen?
      for ( int u=0 ; u<lasers.array.size() ; u++ ) {
        if ( lasers.array[u]->collision( rocks[i]->transedX() ,rocks[i]->transedY() ) ) {
          hitRockIndex = u;
          break;
        }
      }

      // Asteroid von Laser getroffen... teilen und 2 kleinere Brocken erstellen.
      if ( hitRockIndex != -1 ) {
        routinen.genScore( &gameSDL,score+=25 );
        for ( int u=1 ; u<10 ; u++ ) {
          particles.push_back( std::shared_ptr<particle>( new particle( rocks.at(i)->x(),rocks.at(i)->y(), rocks.at(i)->size()*10.0f,
                                                                        0, // min_speed,
                                                                        rocks.at(i)->size()*10.0f, // max speed
                                                                        0, 360 // m_degree - random
                                                                      )
                                                        )
                              );
        }

        float newSize = rocks.at(i)->size() / 2;
        if ( newSize > 0 ) {
          rocks.push_back( std::shared_ptr<rock>( new rock(rocks.at(i)->x(),rocks.at(i)->y(),newSize,rocks.at(i)->magnitude(),lasers.array.at(hitRockIndex)->degree() - 45 ) ) );
          rocks.push_back( std::shared_ptr<rock>( new rock(rocks.at(i)->x(),rocks.at(i)->y(),newSize,rocks.at(i)->magnitude(),lasers.array.at(hitRockIndex)->degree() + 45 ) ) );
        }
        rocks.erase( rocks.begin() + i );
        i--;
      } else {
        SDL_SetRenderDrawColor(gameSDL.renderer(), 0,0,0, SDL_ALPHA_OPAQUE);
        if ( rocks[i]->deceased() ) {
          rocks.erase( rocks.begin() + i );
          i--;
        } else {
          rocks[i]->render(gameSDL.renderer());
        }
      }
    }

    // Partikel verarbeiten und darstellen.  
    for ( int i=0 ; i<particles.size() ; i++ ) {
      if ( particles.at(i)->deceased() ) {
        particles.erase(particles.begin()+i);
        i++;
      } else {
        particles.at(i)->move();
        particles.at(i)->render(gameSDL.renderer());
      } 
    }

    SDL_RenderPresent(gameSDL.renderer());
    SDL_Delay( gameWorld.game_delay );
  }

  return true;
}

int main( int argc,char *argv[] ) {
  sdlEngine gameSDL( (char*) "Asteroids" );

  gameWorld.processArguments(argc,argv);
  gameSDL.use2screen(); // falls 2 Monitore... zeige Programm auf dem rechten.. ;)

  while ( game( gameSDL ) )
    ;

  return 0;
}

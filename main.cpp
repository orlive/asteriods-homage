#include <SDL.h>
#include <iostream>
#include <vector>
#include <cmath>

#define LASER_LIVETIME_MS 1000
#define LASER_SPEED       10
#define LASER_SIZE        20
#define SHIP_RESISTANCE   0.5f
#define SHIP_LASER_WAIT   200

#define PI 3.1415926535f
#define BOGENMASS(a) ( ( PI / 180.0f ) * (float)a )
#define GRAD(a)      ( (float)a / ( PI / 180 ) )

struct {
  int   lastTicks;
  int   elapsed;
  float timeFactor;
  void newLoop() {
    elapsed    = SDL_GetTicks() - lastTicks;
    lastTicks  = SDL_GetTicks();
    timeFactor = (float)elapsed / 1000.0f;
  }
} gameTime;

struct {
  float windowWidth  = 800;
  float windowHeight = 640;

  bool checkBoundaries( float& x , float& y ) {
    bool rc = false;

    while ( x > windowWidth  ) { rc=true; x-=windowWidth;  };
    while ( x < 0            ) { rc=true; x+=windowWidth;  };
    while ( y > windowHeight ) { rc=true; y-=windowHeight; };
    while ( y < 0            ) { rc=true; y+=windowHeight; };

    return rc;
  }
} gameWorld;

class vector {
  private:
    float x;
    float y;

  public:
    vector( float startX , float startY ) {
      x = startX;
      y = startY;
    }

  float rad() {
    float rad = std::atan( y / x );

    if ( x < 0 ) return rad + PI;
    if ( y < 0 ) return rad + PI * 2;

    return rad;
  }

  float degree() {
    float deg = GRAD( rad() ) + 90; 
    while (deg>360.0f) deg-=360.0f;

    return deg;
  }

  float magnitude() {
    return std::sqrtf( x*x + y*y );
  }

};

class force {

  public:
    float vectorX = 0;
    float vectorY = 0;

    void addForce( float degree,float size ) {
      float bogenmass = BOGENMASS(degree);
      vectorX += size * std::sin(bogenmass);
      vectorY -= size * std::cos(bogenmass);
    }

    void stop( float x=0,float y=0 ) {
      vectorX = x;
      vectorY = y;
    }

    void calcOffsets( float& xAdd,float& yAdd ) {
      xAdd = gameTime.timeFactor * vectorX;
      yAdd = gameTime.timeFactor * vectorY;
    }

    void slowDown() {
      vector v(vectorX,vectorY);
      float deg  = v.degree();
      float size = v.magnitude();

      //printf("deg:%f\n",deg);

      size -= ( gameTime.timeFactor * size * SHIP_RESISTANCE );
      if ( size < 0.05f ) size = 0;

      if ( size ) {
        float bogenmass = BOGENMASS(deg);
        vectorX = + size * std::sin(bogenmass);
        vectorY = - size * std::cos(bogenmass);
      }
    }
};

class laser {
  private:
    int width;
    float x,y;
    float degree;
    int livetime;
    int speed;

  public:
    laser(float startX,float startY,float startDegree,float additionalSpeed) {
      x = startX;
      y = startY;
      degree   = startDegree;
      speed    = additionalSpeed;
      livetime = SDL_GetTicks() + LASER_LIVETIME_MS;
    }

    bool render( SDL_Renderer *renderer ) {
      float bogenmass = BOGENMASS(degree);
      float xAdd = -( -(speed+LASER_SPEED)*( 100*std::sin(bogenmass) ) );
      float yAdd =  ( -(speed+LASER_SPEED)*( 100*std::cos(bogenmass) ) );

      x += gameTime.timeFactor * xAdd;
      y += gameTime.timeFactor * yAdd;

      gameWorld.checkBoundaries( x,y );

      float x2 = x - LASER_SIZE*std::sin(bogenmass);
      float y2 = y + LASER_SIZE*std::cos(bogenmass);

      SDL_RenderDrawLine(renderer,x,y,x2,y2);

      return (gameTime.lastTicks > livetime);
    }
};

class ship {

  private:
    int width;
    int height;
    int size;
    std::vector<int> xp;
    std::vector<int> yp;
    force theForce;

  public:
    float x;
    float y;
    float degree;
    float speed;

    ship() {
      x=200.0f;
      y=200.0f;
      size = 50;
      degree = 0;
      speed = 0;

      int offset = -(size/2);

      xp.push_back(         0 ); yp.push_back( offset);
      xp.push_back(  size/5*2 ); yp.push_back( size+offset );
      xp.push_back(-(size/5*2)); yp.push_back( size+offset );
    }

    void thrust() {
      theForce.addForce(degree,gameTime.timeFactor * 200.0f);
    }

    void stop(float x=0,float y=0) {
      theForce.stop(10*x,10*y);
    }

    void rotateLeft() {
      degree -= gameTime.timeFactor * 360;
      if ( degree < 0 ) degree += 360.0f;
    }

    void rotateRight() {
      degree += gameTime.timeFactor * 360;
      if ( degree >= 360 ) degree -= 360.0f;
    }

    void moveShip() {
      float xAdd = 0;
      float yAdd = 0;

      theForce.calcOffsets(xAdd,yAdd);
      x += xAdd;
      y += yAdd;

      gameWorld.checkBoundaries( x,y );

      theForce.slowDown();
    }

    void render( SDL_Renderer *renderer ) {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

      std::vector<float> xn;
      std::vector<float> yn;

      float bogenmass = BOGENMASS(degree);

      for ( int i=0 ; i<xp.size() ; i++ ) {
        xn.push_back( xp[i]*std::cos(bogenmass) - yp[i]*std::sin(bogenmass) );
        yn.push_back( xp[i]*std::sin(bogenmass) + yp[i]*std::cos(bogenmass) );
      }

      SDL_RenderDrawLine(renderer,x+xn[0],y+yn[0],x+xn[1],y+yn[1]);
      SDL_RenderDrawLine(renderer,x+xn[1],y+yn[1],x+xn[2],y+yn[2]);
      SDL_RenderDrawLine(renderer,x+xn[2],y+yn[2],x+xn[0],y+yn[0]);
    }
};

int main() {
  ship aShip;
  std::vector<laser*> lasers;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",SDL_GetError());
		return 1;
	}

	SDL_Cursor *cursor;
	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	if (cursor == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Get the system hand cursor: %s", SDL_GetError());
	}
	SDL_SetCursor(cursor);

	SDL_Window *window;
	SDL_Renderer *renderer;
	if (SDL_CreateWindowAndRenderer(gameWorld.windowWidth, gameWorld.windowHeight, 0, &window, &renderer) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Create window and renderer: %s", SDL_GetError());
		return 1;
	}

	SDL_SetWindowTitle(window,"Space Ship Test");

  SDL_Rect r;
  if (SDL_GetDisplayBounds(0, &r) != 0) {
	  SDL_Log("SDL_GetDisplayBounds failed: %s", SDL_GetError());
	  return 1;
  }

  // falls 2 Monitore... zeige Programm auf dem rechten.. ;)
	printf("SDL_GetNumVideoDisplays() : %d\n",SDL_GetNumVideoDisplays());
  if ( SDL_GetNumVideoDisplays() > 1 ) {
    printf("x/y/w/h:%d/%d/%d/%d\n",r.x,r.y,r.w,r.h);
    int x,y;
    SDL_GetWindowPosition( window,&x,&y);
    printf("x/y:%d/%d\n",x,y);

    SDL_SetWindowPosition( window,x+1980,y);
  }


	SDL_bool done = SDL_FALSE;

  int keyDown;
  unsigned keyDownTicks;

	while (!done) {
    gameTime.newLoop();

    SDL_PumpEvents();
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_SPACE]) {
      static int waitTill;
      if ( gameTime.lastTicks > waitTill ) {
        lasers.push_back( new laser(aShip.x,aShip.y,aShip.degree,aShip.speed) );
        waitTill = gameTime.lastTicks + SHIP_LASER_WAIT;
      }
    }
    if (state[SDL_SCANCODE_RIGHT]) aShip.rotateRight();
    if (state[SDL_SCANCODE_LEFT] ) aShip.rotateLeft();
    if (state[SDL_SCANCODE_UP]   ) aShip.thrust();

    if (state[SDL_SCANCODE_0]    ) aShip.stop();
    if (state[SDL_SCANCODE_1]    ) aShip.stop(1,1);
    if (state[SDL_SCANCODE_2]    ) aShip.stop(1,-1);
    if (state[SDL_SCANCODE_3]    ) aShip.stop(-1,-1);
    if (state[SDL_SCANCODE_4]    ) aShip.stop(-1,1);
 
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
      if ( event.type==SDL_QUIT || ( event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_q ) ) {
			  done = SDL_TRUE;
				break;
      }
    }

    aShip.moveShip( );

    SDL_SetRenderDrawColor(renderer, 255,255,255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
    aShip.render(renderer);

    static int oldSDLTicks;
    
    for ( int i = 0 ; i < lasers.size() ; i++ ) {
      laser *it = lasers.at(i);
      if ( it->render(renderer) ) {
        lasers.erase(lasers.begin()+i);
        i--;
      }
    }

		SDL_RenderPresent(renderer);
    SDL_Delay(10);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

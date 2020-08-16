#ifdef NOEXTERN 
#define EXTERN
#else
#define EXTERN extern
#endif

#ifndef GAMEWORLD_DEFINE
#define GAMEWORLD_DEFINE

#include "vector.h"

typedef struct /* GAMEWORLD */ {

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
    int  charWidth  = 17;
    int  charHeight = 35;
  } display;

  struct /*ship */ { 
    float size       = 25.0f;
    float resistance = 0.5f;
    int   laser_wait = 200;
    int   wait_viable= 3000;

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
  
  void adjustBoundaries( POINT& pos ) {
    while ( pos.x > display.width  ) { pos.x-=display.width;  };
    while ( pos.x < 0              ) { pos.x+=display.width;  };
    while ( pos.y > display.height ) { pos.y-=display.height; };
    while ( pos.y < 0              ) { pos.y+=display.height; };
  }

  bool haveOption( const char option , int argc , char* argv[] ) {
    for ( int i=1 ; i<argc ; i++ ) {
      char*p = argv[i];
      if ( *p!='-' ) continue;
      if ( *(++p)=='-' ) p++;
      if ( *p == option ) return true;
    }
    return false;
  }

  void processArguments( int argc , char* argv[] ) {
    debug          = haveOption( 'd' , argc, argv ) || haveOption( 'v' , argc, argv );
    display.bold   = ! haveOption( 'b' , argc, argv );
    display.mirror = ! haveOption( 'm' , argc, argv );
  }

} GAMEWORLD;

#endif

EXTERN GAMEWORLD gameWorld;

#undef NOEXTERN
#undef EXTERN

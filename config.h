#ifndef CONFIG_DEFINE
#define CONFIG_DEFINE

#include "vector.h"

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}
#include <LuaBridge.h>

class CONFIG {
  public:
    CONFIG();

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
      bool  auto_fire  = false;

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
  
    void toggleRockFreezed();
    void toggleMirrorMode();
    void toggleBoldMode();
  
    void adjustBoundaries( POINT& pos );
    bool haveOption( const char option , int argc , char* argv[] );
    void processArguments( int argc , char* argv[] );

    void getSettings();

  private:
    float getFloatValue( luabridge::LuaRef ref,const char* valueName,float defaultValue );
    int getIntValue( luabridge::LuaRef ref,const char* valueName,int defaultValue );

};

#endif

#ifdef NOEXTERN 
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN CONFIG config;

#undef NOEXTERN
#undef EXTERN

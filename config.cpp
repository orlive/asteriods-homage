#define NOEXTERN 
#include "config.h"
#include "MacBundle.h"

CONFIG::CONFIG() {
  getSettings();
}

void CONFIG::toggleRockFreezed() {
  static int nextToogleRockFreezed;
  if ( time.lastTicks > nextToogleRockFreezed ) {
    nextToogleRockFreezed = time.lastTicks + 500;
    rock.frezzed = ! rock.frezzed;
  }
}

void CONFIG::toggleMirrorMode() {
  static int nextToogleMirrorMode;
  if ( time.lastTicks > nextToogleMirrorMode ) {
    nextToogleMirrorMode = time.lastTicks + 500;
    display.mirror = ! display.mirror;
  }
}

void CONFIG::toggleBoldMode() {
  static int nextToogleBoldMode;
  if ( time.lastTicks > nextToogleBoldMode ) {
    nextToogleBoldMode = time.lastTicks + 500;
    display.bold = ! display.bold;
  }
}
  
void CONFIG::adjustBoundaries( POINT& pos ) {
  while ( pos.x > display.width  ) { pos.x-=display.width;  };
  while ( pos.x < 0              ) { pos.x+=display.width;  };
  while ( pos.y > display.height ) { pos.y-=display.height; };
  while ( pos.y < 0              ) { pos.y+=display.height; };
}

bool CONFIG::haveOption( const char option , int argc , char* argv[] ) {
  for ( int i=1 ; i<argc ; i++ ) {
    char*p = argv[i];
    if ( *p!='-' ) continue;
    if ( *(++p)=='-' ) p++;
    if ( *p == option ) return true;
  }
  return false;
}

void CONFIG::processArguments( int argc , char* argv[] ) {
  if ( haveOption( 'd' , argc, argv ) || haveOption( 'v' , argc, argv ) ) debug = true;
  if ( haveOption( 'b' , argc, argv ) ) display.bold   = false;
  if ( haveOption( 'm' , argc, argv ) ) display.mirror = false;
}

float CONFIG::getFloatValue( luabridge::LuaRef ref,const char* valueName,float defaultValue ) {
  if ( ! ref.isNil() ) {
    luabridge::LuaRef value = ref[valueName];
    if ( ! value.isNil() && value.isNumber() ) {
      return value.cast<float>();
    }
  }
  return defaultValue;
}

int CONFIG::getIntValue( luabridge::LuaRef ref,const char* valueName,int defaultValue ) {
  if ( ! ref.isNil() ) {
    luabridge::LuaRef value = ref[valueName];
    if ( ! value.isNil() ) {
      if ( value.isNumber() ) {
        return value.cast<int>();
      }
      if ( value.isBool() ) {
        return value.cast<bool>();
      }
    }
  }
  return defaultValue;
}

#define INTVALUE(   VAR,TABLE,VALUE ) VAR=getIntValue(   luabridge::getGlobal(L,TABLE),(char*)VALUE,VAR )
#define FLOATVALUE( VAR,TABLE,VALUE ) VAR=getFloatValue( luabridge::getGlobal(L,TABLE),(char*)VALUE,VAR )

#include "sdlEngine.h"

void CONFIG::getSettings() {
  std::string configFile = MacBundle::recourcePath() + "/config.lua";

  lua_State* L = luaL_newstate();
  luaL_dofile(L, configFile.c_str() );

  INTVALUE(   config.debug                           ,"game"    ,"debug" );
  INTVALUE(   config.game_delay                      ,"game"    ,"game_delay" );
  
  INTVALUE(   config.display.width                   ,"display" ,"width" );
  INTVALUE(   config.display.height                  ,"display" ,"height" );
  INTVALUE(   config.display.bold                    ,"display" ,"bold" );
  INTVALUE(   config.display.mirror                  ,"display" ,"mirror" );
  INTVALUE(   config.display.charWidth               ,"display" ,"charWidth" );
  INTVALUE(   config.display.charHeight              ,"display" ,"charHeight" );

  FLOATVALUE( config.ship.size                       ,"ship"    ,"size" );
  FLOATVALUE( config.ship.resistance                 ,"ship"    ,"resistance" );
  INTVALUE(   config.ship.laser_wait                 ,"ship"    ,"laser_wait" );
  INTVALUE(   config.ship.wait_viable                ,"ship"    ,"wait_viable" );
  INTVALUE(   config.ship.auto_fire                  ,"ship"    ,"auto_fire" );
  INTVALUE(   config.ship.thrust_particles.min_speed ,"ship"    ,"thrust_min_speed" );
  INTVALUE(   config.ship.thrust_particles.max_speed ,"ship"    ,"thrust_max_speed" );
  INTVALUE(   config.ship.thrust_particles.min_degree,"ship"    ,"thrust_min_degree" );
  INTVALUE(   config.ship.thrust_particles.max_degree,"ship"    ,"thrust_max_degree" );

  INTVALUE(   config.laser.lifespan                  ,"laser"   ,"lifespan" );
  INTVALUE(   config.laser.speed                     ,"laser"   ,"speed" );
  INTVALUE(   config.laser.size                      ,"laser"   ,"size" );

  INTVALUE(   config.particle.lifespan               ,"particle","lifespan" );

  INTVALUE(   config.rock.lifespan                   ,"rock"    ,"lifespan" );
}


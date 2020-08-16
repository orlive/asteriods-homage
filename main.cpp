#include <memory>
#define NOEXTERN
#include "gameWorld.h"
#define NOEXTERN
#include "tools.h"
#include "sdlEngine.h"
#include "vector.h"
#include "force.h"
#include "base.h"
#include "rock.h"
#include "laser.h"
#include "particle.h"
#include "ship.h"

struct /* LASERS */ {
  std::vector< std::shared_ptr<laser> > array;

  void newLaser(ship& aShip,sdlEngine& sdl) {
    static int waitTill;
    if ( gameWorld.time.lastTicks > waitTill ) {
      array.push_back( std::shared_ptr<laser>( new laser(aShip.x(),aShip.y(),aShip.getDegree()) ) );
      waitTill = gameWorld.time.lastTicks + gameWorld.ship.laser_wait;
      sdl.playSound(0);
      //SDL_FreeWAV(wav_buffer);
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
  UIText textGameOver;

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

  void genGameOver( sdlEngine* engine ) {
    textGameOver = { engine->genTexture( (char*)"GAME OVER") , 9 };
  }

  void drawGameOver( sdlEngine* engine ) {
    engine->drawTexture( gameWorld.display.width / 2 - textGameOver.width*gameWorld.display.charWidth/2,
                         gameWorld.display.height / 2 - gameWorld.display.charHeight/2,
                         textGameOver.width*gameWorld.display.charWidth,
                         gameWorld.display.charHeight,
                         textGameOver.texture );
  }

  void drawTexts( sdlEngine* engine ) {
    engine->drawTexture(   1,1,textLevel.width*gameWorld.display.charWidth,gameWorld.display.charHeight,textLevel.texture );
    engine->drawTexture( 250,1,textScore.width*gameWorld.display.charWidth,gameWorld.display.charHeight,textScore.texture );
    engine->drawTexture( 500,1,textLives.width*gameWorld.display.charWidth,gameWorld.display.charHeight,textLives.texture );
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
    
    engine->drawTexture( 740,1,textFPS.width*gameWorld.display.charWidth/3*2,gameWorld.display.charHeight/3*2,textFPS.texture );
  }

} routinen;

bool game( sdlEngine& gameSDL ) {
  ship aShip;
  std::vector< std::shared_ptr<rock> > rocks;
  std::vector< std::shared_ptr<particle> > particles;
  int level = 0;
  int score = 0;
  int lives = 3;

  gameSDL.playSound(4);

  routinen.genLevel( &gameSDL,level );
  routinen.genScore( &gameSDL,score );
  routinen.genLives( &gameSDL,lives );
  routinen.genGameOver( &gameSDL );

  bool spacePressed = false;

  while ( true ) {
    gameSDL.newLoop( gameWorld.time.elapsed,gameWorld.time.lastTicks,gameWorld.time.timeFactor );
    gameSDL.newInput();

    const Uint8* state = gameSDL.state();

    if ( state[SDL_SCANCODE_SPACE] && !spacePressed ) {
      if ( ! aShip.isDestroyed() ) {
        lasers.newLaser(aShip,gameSDL); 
      } else if ( lives>0 ) {
        if ( aShip.renew() ) {
          gameSDL.playSound(4);
        }  
      }
    }
    spacePressed = ( state[SDL_SCANCODE_SPACE] );

    if ( state[SDL_SCANCODE_RIGHT] ) aShip.rotateRight();
    if ( state[SDL_SCANCODE_LEFT]  ) aShip.rotateLeft();
    if ( state[SDL_SCANCODE_UP]    ) aShip.thrust(particles);
    if ( state[SDL_SCANCODE_R]     ) if ( aShip.isDestroyed() && lives<=0 ) return true;

    if ( gameWorld.debug ) {
      if ( state[SDL_SCANCODE_0] ) aShip.stop(); // TODO - entfernen, wenn fertig
      if ( state[SDL_SCANCODE_F] ) gameWorld.toggleRockFreezed(); // TODO - entfernen, wenn fertig
      if ( state[SDL_SCANCODE_M] ) gameWorld.toggleMirrorMode(); // TODO - entfernen, wenn fertig
      if ( state[SDL_SCANCODE_B] ) gameWorld.toggleBoldMode(); // TODO - enfernen, wenn fertig
      if ( state[SDL_SCANCODE_8] ) aShip.resize( ++gameWorld.ship.size );
      if ( state[SDL_SCANCODE_9] ) aShip.resize( --gameWorld.ship.size );
      if ( state[SDL_SCANCODE_1] ) printf( "(%05ld) FPS:%f\n",rocks.size(),1000.0f/gameWorld.time.elapsed );
      if ( state[SDL_SCANCODE_D] ) { static int wait; 
                                     if ( lives>0 && gameWorld.time.lastTicks > wait) {
                                       routinen.genLives( &gameSDL , --lives ); 
                                       wait=gameWorld.time.lastTicks+500; 
                                     } 
                                   }
    }

    if ( gameSDL.haveQuitEvent() ) return false;

    SDL_SetRenderDrawColor(gameSDL.renderer(), 0,0,0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(gameSDL.renderer());

    routinen.drawTexts( &gameSDL );
    routinen.drawFPS( &gameSDL );

    if ( lives <= 0 ) {
      routinen.drawGameOver( &gameSDL );
    }

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
      if ( aShip.isViable() ) {
        if ( ! aShip.isDestroyed() ) {
          if ( aShip.collision( rocks[i]->transformed() ) ) {
            aShip.destroy(particles);
            routinen.genLives( &gameSDL , --lives );
            gameSDL.playSound(3);
          }
        }
      }

      // -> von einem Laser getroffen?
      for ( int u=0 ; u<lasers.array.size() ; u++ ) {
        if ( lasers.array[u]->collision( rocks[i]->transformed() ) ) {
          hitRockIndex = u;
          break;
        }
      }

      // Asteroid von Laser getroffen... teilen und 2 kleinere Brocken erstellen.
      if ( hitRockIndex != -1 ) {
        gameSDL.playSound(1);
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
  sdlEngine gameSDL( (char*) "Asteroids",gameWorld.display.width,gameWorld.display.height );

  std::vector<std::string> sounds = {
    "assets/sounds/344276__nsstudios__laser3.wav", // laser
    "assets/sounds/384113__microsoftsam__bang01.wav", // rock hit
    "assets/sounds/95749__cgeffex__ship-artillery-blast-classic.wav", // ship explosion
    "assets/sounds/462189__tolerabledruid6__8-bit-atari-boom.wav", // ship explosion - alternativ
    "assets/sounds/323134__cognito-perceptu__atari-style-vibration-4.wav" // ship restart
  };
  gameSDL.loadSounds( sounds );

  gameWorld.processArguments(argc,argv);

  if ( gameWorld.haveOption('2',argc,argv) ) {
    gameSDL.use2screen(); // falls 2 Monitore... zeige Programm auf dem rechten.. ;)
  }

  while ( game( gameSDL ) )
    ;

  return 0;
}

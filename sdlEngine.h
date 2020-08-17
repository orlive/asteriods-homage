#ifndef SDLENGINE_DEFINE
#define SDLENGINE_DEFINE

#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cmath>

class sdlEngine {
  private:
    SDL_Window   *m_window;
    SDL_Renderer *m_renderer;
    SDL_Cursor   *m_cursor;
    TTF_Font     *m_font;
    SDL_Color     m_fontColor = {255, 255, 255, 255};

    const Uint8  *m_state;

    typedef struct {
      std::string   name;
      SDL_AudioSpec wav_spec;
      Uint32        wav_length;
      Uint8        *wav_buffer;
      SDL_AudioDeviceID deviceId;
    } SOUND;

    std::vector<SOUND> m_sounds;

  public:
    sdlEngine( const char *title,int width,int height );
    ~sdlEngine();
    void newLoop( int& timeElapsed , int& timeLastTicks, float& timeFactor );
    const Uint8* state();
    SDL_Renderer* renderer();
    bool haveQuitEvent();
    void newInput();
    void use2screen();
    SDL_Texture* genTexture( const char* text );
    void drawTexture( int x,int y,int w,int h,SDL_Texture* texture );
    void drawText( int x,int y,int w,int h,const char* text );
    void loadSounds( std::vector<std::string> sounds );
    void playSound( int index );
};

#endif
#define SDLENGINE_DEFINE

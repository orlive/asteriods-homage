#include "sdlEngine.h"

sdlEngine::sdlEngine( const char *title,int width,int height ) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",SDL_GetError());
    return;
  }
  SDL_Init(SDL_INIT_AUDIO);

  if ( ( m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND) ) == NULL ) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Get the system hand cursor: %s", SDL_GetError());
    return;
  }
  SDL_SetCursor(m_cursor);

  if (SDL_CreateWindowAndRenderer(width,height, 0, &m_window, &m_renderer) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Create window and renderer: %s", SDL_GetError());
    return;
  }

  SDL_SetWindowTitle( m_window , title );
  if ( TTF_Init() < 0 ) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL_ttf: %s",TTF_GetError());
    return;
  }
    
  m_font = TTF_OpenFont("assets/Hack-Bold.ttf", 20);
  if (m_font == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Open font: %s",TTF_GetError());
    return;
  }
}

sdlEngine::~sdlEngine() {
  TTF_CloseFont(m_font);
  SDL_DestroyRenderer(m_renderer);
  SDL_DestroyWindow(m_window);
  for ( std::vector<SOUND>::iterator it=m_sounds.begin() ; it!=m_sounds.end() ; it++ ) {
    SDL_FreeWAV( it->wav_buffer );
  }

  SDL_Quit();
}

void sdlEngine::newLoop( int& timeElapsed , int& timeLastTicks, float& timeFactor ) {
  timeElapsed   = SDL_GetTicks() - timeLastTicks;
  timeLastTicks = SDL_GetTicks();
  timeFactor    = (float)timeElapsed / 1000.0f;
}

const Uint8* sdlEngine::state() {
  return m_state;
}

SDL_Renderer* sdlEngine::renderer() {
  return m_renderer;
}

bool sdlEngine::haveQuitEvent() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if ( event.type==SDL_QUIT || ( event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_q ) ) {
      return true;
    }
  }
  return false;
}
  
void sdlEngine::newInput() {
  SDL_PumpEvents();
  this->m_state = SDL_GetKeyboardState(NULL);
}

void sdlEngine::use2screen() {
  printf("SDL_GetNumVideoDisplays() : %d\n",SDL_GetNumVideoDisplays());
  if ( SDL_GetNumVideoDisplays() > 1 ) {
    int x,y;
    SDL_GetWindowPosition( m_window,&x,&y);
    SDL_SetWindowPosition( m_window,x+1980,y);
    printf("x/y:%d/%d\n",x,y);
  }
}

SDL_Texture* sdlEngine::genTexture( const char* text ) {
  SDL_Surface *surface = TTF_RenderText_Blended( m_font,text,m_fontColor );
  SDL_Texture *texture = SDL_CreateTextureFromSurface(m_renderer, surface);
  SDL_FreeSurface(surface);

  return texture;
}

void sdlEngine::drawTexture( int x,int y,int w,int h,SDL_Texture* texture ) {
  if ( texture ) {
    SDL_Rect dest = { .x = x , .y = y , .w = w , .h = h };
    SDL_RenderCopy(m_renderer, texture, NULL, &dest);
  }
}

void sdlEngine::drawText( int x,int y,int w,int h,const char* text ) {
  SDL_Texture *texture = genTexture( text );
  drawTexture( x,y,w,h,texture );
  SDL_DestroyTexture(texture);
}

void sdlEngine::loadSounds( std::vector<std::string> sounds ) {
  for ( std::vector<std::string>::iterator i=sounds.begin() ; i!=sounds.end() ; i++ ) {
    SOUND s = { .name = *i };
    if (SDL_LoadWAV( i->c_str(), &s.wav_spec, &s.wav_buffer, &s.wav_length) == NULL) {
      fprintf(stderr, "Could not open %s: %s\n",i->c_str(), SDL_GetError());
      exit(1);
    }

    s.deviceId = SDL_OpenAudioDevice(NULL, 0, &s.wav_spec, NULL, 0);
    m_sounds.push_back( s );
  }
}

void sdlEngine::playSound( int index ) {
  if ( index>=0 && index<(int)m_sounds.size() ) {
    SOUND s = m_sounds.at(index);
    SDL_ClearQueuedAudio(s.deviceId);
    SDL_QueueAudio(s.deviceId,s.wav_buffer,s.wav_length);
    SDL_PauseAudioDevice(s.deviceId, 0);
    SDL_Delay(5);
  }
}


#ifndef gfx_sdl_h
#define gfx_sdl_h

#include <SDL.h>
#include "gfx.h"
#include "debug.h"

class Sdlgfx: public Gfx
{
	SDL_Window*  window;
	SDL_Surface* surface;
	public:
	Sdlgfx( unsigned width, unsigned height );
	void blit( void* pixels, uint32_t width, uint32_t height );
	void update( void );
	void delay( unsigned milliseconds );
	~Sdlgfx( void );
};

#endif
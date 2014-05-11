#include "gfx_sdl.h"
Sdlgfx::Sdlgfx( unsigned width, unsigned height )
{
	SDL_Init( SDL_INIT_EVERYTHING );
	this->width = width;
	this->height = height;
	window = SDL_CreateWindow( title, 10, 10, width, height, SDL_WINDOW_RESIZABLE );
	if(!window)
	{
		dprintf( 0, "Could not create a window: %s\n", SDL_GetError() );
	}
	surface = SDL_GetWindowSurface(window);
}

void Sdlgfx::blit( void* pixels, uint32_t width, uint32_t height )
{	
	SDL_Surface* second;
	width = width < this->width ? width : this->width;
	height = height < this->height ? height : this->height;
	second = SDL_CreateRGBSurfaceFrom(
		pixels,
		width,
		height,
		32,
		width*sizeof(uint32_t),
		0x000000ff,
		0x0000ff00,
		0x00ff0000,
		0xff000000
	);
	SDL_BlitSurface( second, NULL, surface, NULL );
	SDL_FreeSurface( second );
}

void Sdlgfx::update( void )
{
	SDL_UpdateWindowSurface( window );
}
void Sdlgfx::delay( unsigned milliseconds )
{
	SDL_Delay( milliseconds );
}

Sdlgfx::~Sdlgfx( void )
{
	dprintf( 1, "Tearing down SDL context\n" );
	SDL_DestroyWindow( window );
	SDL_Quit( );
}
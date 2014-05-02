#include <vector>
#include "control_sdl.h"

SDLControl::SDLControl( RTContext* rtcontext )
{
	context = rtcontext;
	w = a = s = d = ctrl = shift = lmouse = rmouse = false;
	SDL_GetMouseState( &nx, &ny );
	SDL_GetMouseState( &ox, &oy );
}

void SDLControl::keyboardAction( SDL_KeyboardEvent event )
{
	bool pressed = event.type == SDL_KEYDOWN;
	if( event.repeat )
	{
		return;
	}
	switch( event.keysym.sym )
	{
		case SDLK_w:
			dprintf( 9, "w press event\n" );
			w = pressed;
			break;
			
		case SDLK_a:
			dprintf( 9, "a press event\n" );
			a = pressed;
			break;
			
		case SDLK_s:
			dprintf( 9, "s press event\n" );
			s = pressed;
			break;
			
		case SDLK_d:
			dprintf( 9, "d press event\n" );
			d = pressed;
			break;
			
		case SDLK_LCTRL:
			dprintf( 9, "lctrl press event\n" );
			ctrl = pressed;
			break;
			
		case SDLK_LSHIFT:
			dprintf( 9, "lshift press event\n" );
			shift = pressed;
			break;
	}

}

void SDLControl::move( void )
{
	float mod = .3f;
	if(shift) mod *= 3.f;
	if(ctrl) mod /= 3.f;
	if(w) context->step( mod );
	if(a) context->strafe( mod );
	if(s) context->step( -mod );
	if(d) context->strafe( -mod );
	int dx = nx - ox;
	int dy = ny - oy;
	ox = nx;
	oy = ny;
	if(rmouse) context->mouse( dx, dy );
}

void SDLControl::mouseAction( SDL_MouseButtonEvent event )
{
	if( event.button == SDL_BUTTON_RIGHT )
	{
		rmouse = event.state == SDL_PRESSED;
	}
}

void SDLControl::mouseMotion( SDL_MouseMotionEvent event )
{
	nx = event.x;
	ny = event.y;
}

void SDLControl::actions( void )
{
	std::vector<SDL_Event> rendront;
	SDL_Event event;
	while( SDL_PollEvent( &event ) )
	{
		switch( event.type )
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				keyboardAction( event.key );
				break;
				
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				mouseAction( event.button );
				break;
				
			case SDL_MOUSEMOTION:
				mouseMotion( event.motion );
				break;
				
			default:
				rendront.push_back( event );
				break;
		}
	}
	
	for( unsigned i = 0; i < rendront.size(); i++ )
	{
		SDL_PushEvent( &rendront[i] );
	}
	
	time = SDL_GetTicks( );
	
	move();
}

void SDLControl::keyboardActions( void )
{
	std::vector<SDL_Event> rendront;
	SDL_Event event;
	while( SDL_PollEvent( &event ) )
	{
		switch( event.type )
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				keyboardAction( event.key );
				break;
				
			default:
				rendront.push_back( event );
				break;
		}
		
	}
	
	for( unsigned i = 0; i < rendront.size(); i++ )
	{
		SDL_PushEvent( &rendront[i] );
	}
}

void SDLControl::mouseActions( void )
{
	std::vector<SDL_Event> rendront;
	SDL_Event event;
	while( SDL_PollEvent( &event ) )
	{
		switch( event.type )
		{
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				mouseAction( event.button );
				break;
				
			case SDL_MOUSEMOTION:
				mouseMotion( event.motion );
				break;
				
			default:
				rendront.push_back( event );
				break;
		}
		
	}
	
	for( unsigned i = 0; i < rendront.size(); i++ )
	{
		SDL_PushEvent( &rendront[i] );
	}
}

unsigned SDLControl::postactions( void )
{
	return SDL_GetTicks() - time;
}
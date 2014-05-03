#include <CL/cl.h>
#include <vector>
#include <stdint.h>
#include "debug.h"
#include "clstuff.h"
#include "geometry.h"
#include "gfx_sdl.h"
#include "control_sdl.h"

Gfx* graphics;
Control* control;
RTContext* context;
bool running;

int main(int argc, char *argv[])
{

	int num_devices = clinit();
	if(!num_devices)
	{
		dprintf( 0, "No GPU devices available\n");
		return 0;
	}
	context = new RTContext();
	DeviceContext device( 0 );
	context->registerDeviceContext( device );
	graphics = new Sdlgfx( 1024, 768 );
	control = new SDLControl( context );
	
	context->addGeometry( make_sphere( make_float3( 3.f, -10.f, 3.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 4.f, -10.f, 3.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 5.f, -10.f, 3.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 6.f, -10.f, 3.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 7.f, -10.f, 3.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 8.f, -10.f, 3.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 3.f, -10.f, 4.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 4.f, -10.f, 5.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 5.f, -10.f, 6.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 6.f, -10.f, 7.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 7.f, -10.f, 8.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 8.f, -10.f, 9.f ), 1.f ) );
	
	
	context->addGeometry( make_box( make_float3( -4.f, 5.f, 4.f ), make_float3( -6.f, 5.f, 4.f ), make_float3( -4.f, 5.f, 2.f ), 1 ) );

	running = true;
	while( running ){
		control->actions( );
		void* test = context->trace( 1024, 768 );
		graphics->blit( test, 1024, 768 );
		graphics->update( );
		free(test);
		unsigned time = control->postactions( );
		if( time < 20 )
		{
			graphics->delay( 20 - time );
		}
	}
	
	return 0;
}
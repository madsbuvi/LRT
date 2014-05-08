#include <CL/cl.h>
#include <vector>
#include <stdint.h>
#include "debug.h"
#include "clstuff.h"
#include "geometry.h"
#include "gfx_sdl.h"
#include "control_sdl.h"
#include "shaders.h"

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
	
	// The Shader family of object stores a list of all shaders made, and uses the first in the list as default shader.
	// Can be overwritten by a call to (ShaderObject).setAsDefaultShader() on any shader object.
	// Note also that i'm a retard so we get BGR instead of RGB
	new SimpleDiffusionShader( make_float3( 0.f, 1.f, 0.f ) );
	
	context->addGeometry( make_sphere( make_float3( 3.f, 10.f, 3.f ), 1.f ),
						new SimpleDiffusionShader( make_float3( 0.55f, 0.55f, 0.44f ) ) );
	
	context->addGeometry( make_sphere( make_float3( 4.f, 10.f, 3.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 5.f, 10.f, 3.f ), 1.f ),
						new SimpleDiffusionShader( make_float3( 0.50f, 0.50f, 0.55f ) ) );
	context->addGeometry( make_sphere( make_float3( 6.f, 10.f, 3.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 7.f, 10.f, 3.f ), 1.f ),
						new SimpleDiffusionShader( make_float3( 0.45f, 0.45f, 0.66f ) ) );
	context->addGeometry( make_sphere( make_float3( 8.f, 10.f, 3.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 3.f, 10.f, 4.f ), 1.f ),
						new SimpleDiffusionShader( make_float3( 0.40f, 0.40f, 0.77f ) ) );
	context->addGeometry( make_sphere( make_float3( 4.f, 10.f, 5.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 5.f, 10.f, 6.f ), 1.f ),
						new SimpleDiffusionShader( make_float3( 0.35f, 0.35f, 0.88f ) ) );
	context->addGeometry( make_sphere( make_float3( 6.f, 10.f, 7.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3( 7.f, 10.f, 8.f ), 1.f ),
						new SimpleDiffusionShader( make_float3( 0.30f, 0.30f, 0.99f ) ) );
	context->addGeometry( make_sphere( make_float3( 8.f, 10.f, 9.f ), 1.f ) );
	context->addGeometry( make_sphere( make_float3(0.f, 0.f, -1.5f), .5f ),
						new SimpleDiffusionShader( make_float3( 0.25f, 0.25f, 0.33f ) ) );
	
	context->addGeometry( make_AAB( make_float3(1.f, 0.f, -0.5f), make_float3(2.f, -1.f, 0.5f) ),
						new SimpleDiffusionShader( make_float3( 0.11f, 0.55f, 0.44f ) ) );
	
	context->addGeometry( make_triangle( make_float3(-1.f, 0.f, 0.f), make_float3(-2.f, -1.f, 0.f), make_float3( -1.5f, -1.f, 1.f ) ),
						new SimpleDiffusionShader( make_float3( 0.55f, 0.99f, 0.44f ) ) );
	
	context->addGeometry( make_box( make_float3( 2.f, -2.f, 2.f ), make_float3( 4.f, -2.f, 2.f ), make_float3( 2.f, -2.f, 4.f ), 2.f ) );
	context->addGeometry( make_box( make_float3( -4.f, -5.f, 4.f ), make_float3( -6.f, -5.f, 4.f ), make_float3( -4.f, -5.f, 2.f ), 1 ),
						new SimpleDiffusionShader( make_float3( 0.11f, 0.11f, 0.11f ) ) );
	
	
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
#include <CL/cl.h>
#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <iostream>
#include "common.h"
#include "debug.h"
#include "clstuff.h"
#include "geometry.h"
#include "gfx_glfw.h"
#include "control_glfw.h"
#include "shaders.h"


#define WIDTH 1920
#define HEIGHT 1020

Glfwgfx* graphics;
Control* control;
RTContext* context;
bool running;


#ifdef USE_ROCKET

static void exitListen( void* dummy )
{
	running = false;
}

#endif

int main(int argc, char *argv[])
{
	double frames = 0;
	unsigned start;
	int num_devices = clinit();
	
	if(!num_devices)
	{
		dprintf( 0, "No GPU devices available\n");
		return 0;
	}
	context = new RTContext();
	graphics = new Glfwgfx( WIDTH, HEIGHT, context );
	DeviceContext device( 0, graphics->getWindow() );
	context->registerDeviceContext( device );
	
#ifdef USE_ROCKET
	initRocketMenu();
	registerListener( "exit", exitListen, NULL );
#endif

	
	control = new GlfwControl( context, graphics
#ifdef USE_ROCKET
	, rContext 
#endif
	);
	
	//Texture* stardust = new Texture("Textures/stardust.png");
	Texture* cgo = new Texture("Textures/congruent_outline.png");
	//Texture* cracked = new Texture("Textures/crackedmud1.jpg");
	Texture* L3Dgreen = new Texture("Textures/Green-L3D.png");
	Texture* L3Dblue = new Texture("Textures/Blue-L3D.png");
	Texture* L3Dbrown = new Texture("Textures/Brown-L3D.png");
	Texture* L3Dredbase = new Texture("Textures/Red-Base-L3D.png");
	
	// The Shader family of object stores a list of all shaders made, and uses the first in the list as default shader.
	// Can be overwritten by a call to (ShaderObject).setAsDefaultShader() on any shader object.
	// Note also that i'm a retard so we get BGR instead of RGB
	new SimpleDiffusionShaderTex( cgo );

	forrange2( i, 0, 3, j, 0, 3 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i)*4.f, -1.f, float(j)*4.f ), make_float3( float(i+1)*4.f, 0.f, float(j+1)*4.f ) ),
							new SimpleDiffusionShaderTex( L3Dredbase ) );
	}
	
	forrange2( i, 2, 9, j, 2, 9 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), 0.f, float(j) ) ),
							new SimpleDiffusionShaderTex( L3Dblue ) );
	}
	forrange2( i, 3, 8, j, 3, 8 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), 1.f, float(j) ) ),
							new SimpleDiffusionShaderTex( L3Dblue ) );
	}
	
	forrange2( i, 3, 5, j, 3, 8 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), 2.f, float(j) ) ),
							new SimpleDiffusionShaderTex( L3Dblue ) );
	}
	forrange2( i, 5, 9, j, 3, 8 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), 2.f, float(j) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	
	forrange3( i, 2, 9, j, 2, 9, k, 4, 9 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), float(k), float(j) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange2( i, 4, 9, j, 2, 9)
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), float(3), float(j) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	
	forrange( i, 3, 8 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(3), float(3), float(i) ) ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange3( i, 3, 8, j, 0, 2, k, 3, 7 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), float(k), float(j) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	
	forrange3( i, 3, 8, j, 9, 11, k, 3, 7 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), float(k), float(j) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	
	forrange3( i, -1, 3, j, 3, 8, k, 3, 6 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), float(k), float(j) ) ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	
	forrange( i, 4, 7 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(0), float(6), float(i) ) ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	
	forrange( i, 4, 7 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(-2), float(3), float(i) ) ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange2( i, -1, 3, j, 4, 7 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), 2.f, float(j) ) ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	context->addGeometry( new Geometry_AAB( make_float3( float(-1), float(6), float(5) ) ),
						new SimpleDiffusionShaderTex( L3Dbrown ) );
	
	
	forrange2( i, -1, 2, j, 4, 7 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), float(7), float(j) ) ),
						new SimpleDiffusionShaderTex( L3Dgreen) );
	}
	forrange2( i, 0, 2, j, 4, 7 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), float(8), float(j) ) ),
						new SimpleDiffusionShaderTex( L3Dgreen) );
	}
	forrange2( i, 1, 2, j, 4, 7 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), float(9), float(j) ) ),
						new SimpleDiffusionShaderTex( L3Dgreen) );
	}
	forrange2( i, 2, 7, j, 4, 8 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), float(10), float(j) ) ),
						new SimpleDiffusionShaderTex( L3Dgreen) );
	}
	forrange2( i, 2, 7, j, 2, 8 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(i), float(9), float(j) ) ),
						new SimpleDiffusionShaderTex( L3Dgreen) );
	}
	forrange( i, 6, 9 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(1), float(i), float(7) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 6, 9 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(1), float(i), float(3) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	

	forrange( i, 6, 9 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(1), float(i), float(2) ), SW ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}

	forrange( i, 7, 9 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(0), float(i), float(3) ), SW ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}

	forrange( i, 6, 9 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(1), float(i), float(8) ), SE ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 7, 9 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(0), float(i), float(7) ), SE ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}

	forrange( i, 3, 7 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(i), float(4), float(-1) ), UW ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 3, 7 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(i), float(3), float(-1) ), DW ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 3, 7 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(i), float(2), float(0) ), DW ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 3, 7 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(i), float(2), float(1) ), DE ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	
	
	forrange( i, 3, 7 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(i), float(4), float(11) ), UE ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 3, 7 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(i), float(3), float(11) ), DE ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 3, 7 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(i), float(2), float(9) ), DW ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 3, 7 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(i), float(2), float(10) ), DE ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	
	
	forrange( i, 4, 7 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(-2), float(4), float(i) ), US ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange( i, 4, 7 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(-2), float(2), float(i) ), DS ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange( i, 4, 7 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(-1), float(1), float(i) ), DS ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange( i, 4, 7 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(1), float(1), float(i) ), DN ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange( i, 4, 7 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(0), float(1), float(i) ) ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange( i, -1, 3 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(i), float(2), float(3) ), DW ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange( i, -1, 3 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(i), float(2), float(7) ), DE ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	
	
	
	
	
	forrange( i, 3, 8 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(9), float(2), float(i) ), DN ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 4, 7 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(10), float(3), float(i) ), DN ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	
	
	forrange2( i, 3, 8, j, 3, 8 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(9), float(j), float(i) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange2( i, 4, 7, j, 4, 8 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(10), float(j), float(i) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 4, 9 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(8), float(i), float(1) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 5, 9 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(9), float(i), float(2) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 4, 9 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(8), float(i), float(9) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 5, 9 )
	{
		context->addGeometry( new Geometry_AAB( make_float3( float(9), float(i), float(8) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 4, 8 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(10), float(i), float(3) ), NW ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 4, 8 )
	{
		context->addGeometry( new Geometry_AAP( make_float3( float(10), float(i), float(7) ), NE ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	
	Image* limage = new Image("test.png");
	//MWMenu* testmenu = new MWMenu( make_int2(0,0), make_int2(256,256), limage);
	
	running = true;
	start = control->timeMillis();
	while( running ){
		// Treat all control inputs made since last update
		control->actions( );
		
		// Perform a new ray trace and update graphics buffer
		void* test = context->trace( WIDTH, HEIGHT );
		glClear(GL_COLOR_BUFFER_BIT);
		graphics->blit( test, WIDTH, HEIGHT );
		//testmenu->draw( graphics->getWindow() );
		
#ifdef USE_ROCKET
		// Update the user interface
		rocketLoop();

#endif
		// Blit the buffer
		graphics->update( );
		

		frames++;
		free(test);
		// Perform any actions that need performing between frames but before going to sleep.
		unsigned time = control->postactions( );
		
		// Sleep to make sure framerate doesn't explode
		if( time < 10 )
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10 - time));
		}
		glfwPollEvents();
		double elapsed = control->timeMillis() - start;
		if(unsigned(frames)%10==0)printf("Fps averaging to %g/s\r", (frames / elapsed)*1000.);
		fflush(stdout);
	}
	
	// Shutdown Rocket.
#ifdef USE_ROCKET
	rContext->RemoveReference();
	Rocket::Core::Shutdown();
	Shell::CloseWindow();
	Shell::Shutdown();
#endif
	return 0;
}

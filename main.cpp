#include <CL/cl.h>
#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include "debug.h"
#include "clstuff.h"
#include "geometry.h"
#include "gfx_sdl.h"
#include "gfx_glfw.h"
#include "control_sdl.h"
#include "control_glfw.h"
#include "shaders.h"
#include "util.h"

#define WIDTH 1024
#define HEIGHT 768

Glfwgfx* graphics;
Control* control;
RTContext* context;
bool running;


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
	graphics = new Glfwgfx( WIDTH, HEIGHT );
	control = new GlfwControl( context, graphics );
	DeviceContext device( 0, graphics->getWindow() );
	context->registerDeviceContext( device );
	
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
		context->addGeometry( make_AAB( make_float3( float(i)*4.f, -1.f, float(j)*4.f ), make_float3( float(i+1)*4.f, 0.f, float(j+1)*4.f ) ),
							new SimpleDiffusionShaderTex( L3Dredbase ) );
	}
	forrange2( i, 2, 9, j, 2, 9 )
	{
		context->addGeometry( make_AAB( make_float3( float(i), 0.f, float(j) ), make_float3( float(i+1), 1.f, float(j+1) ) ),
							new SimpleDiffusionShaderTex( L3Dblue ) );
	}
	forrange2( i, 3, 8, j, 3, 8 )
	{
		context->addGeometry( make_AAB( make_float3( float(i), 1.f, float(j) ), make_float3( float(i+1), 2.f, float(j+1) ) ),
							new SimpleDiffusionShaderTex( L3Dblue ) );
	}
	forrange2( i, 3, 5, j, 3, 8 )
	{
		context->addGeometry( make_AAB( make_float3( float(i), 2.f, float(j) ), make_float3( float(i+1), 3.f, float(j+1) ) ),
							new SimpleDiffusionShaderTex( L3Dblue ) );
	}
	forrange2( i, 5, 9, j, 3, 8 )
	{
		context->addGeometry( make_AAB( make_float3( float(i), 2.f, float(j) ), make_float3( float(i+1), 3.f, float(j+1) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange3( i, 2, 9, j, 2, 9, k, 4, 9 )
	{
		context->addGeometry( make_AAB( make_float3( float(i), float(k), float(j) ), make_float3( float(i+1), float(k+1), float(j+1) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange2( i, 4, 9, j, 2, 9)
	{
		context->addGeometry( make_AAB( make_float3( float(i), float(3), float(j) ), make_float3( float(i+1), float(3+1), float(j+1) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 3, 8 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(3), float(3), float(i) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	
	forrange3( i, 3, 8, j, 0, 2, k, 3, 7 )
	{
		context->addGeometry( make_AAB( make_float3( float(i), float(k), float(j) ), make_float3( float(i+1), float(k+1), float(j+1) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	
	forrange3( i, 3, 8, j, 9, 11, k, 3, 7 )
	{
		context->addGeometry( make_AAB( make_float3( float(i), float(k), float(j) ), make_float3( float(i+1), float(k+1), float(j+1) ) ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	
	forrange3( i, -1, 3, j, 3, 8, k, 3, 6 )
	{
		context->addGeometry( make_AAB( make_float3( float(i), float(k), float(j) ), make_float3( float(i+1), float(k+1), float(j+1) ) ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	
	forrange( i, 4, 7 )
	{
		context->addGeometry( make_AAB( make_float3( float(0), float(6), float(i) ), make_float3( float(1), float(7), float(i+1) ) ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	
	forrange( i, 4, 7 )
	{
		context->addGeometry( make_AAB( make_float3( float(-2), float(3), float(i) ), make_float3( float(-1), float(4), float(i+1) ) ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange2( i, -1, 3, j, 4, 7 )
	{
		context->addGeometry( make_AAB( make_float3( float(i), 2.f, float(j) ), make_float3( float(i+1), 3.f, float(j+1) ) ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	context->addGeometry( make_AAB( make_float3( float(-1), float(6), float(5) ), make_float3( float(0), float(7), float(6) ) ),
						new SimpleDiffusionShaderTex( L3Dbrown ) );
	forrange2( i, -1, 2, j, 4, 7 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(i), float(7), float(j) ), 1.f ),
						new SimpleDiffusionShaderTex( L3Dgreen) );
	}
	forrange2( i, 0, 2, j, 4, 7 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(i), float(8), float(j) ), 1.f ),
						new SimpleDiffusionShaderTex( L3Dgreen) );
	}
	forrange2( i, 1, 2, j, 4, 7 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(i), float(9), float(j) ), 1.f ),
						new SimpleDiffusionShaderTex( L3Dgreen) );
	}
	forrange2( i, 2, 7, j, 4, 8 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(i), float(10), float(j) ), 1.f ),
						new SimpleDiffusionShaderTex( L3Dgreen) );
	}
	forrange2( i, 2, 7, j, 2, 8 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(i), float(9), float(j) ), 1.f ),
						new SimpleDiffusionShaderTex( L3Dgreen) );
	}
	forrange( i, 6, 9 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(1), float(i), float(7) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 6, 9 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(1), float(i), float(3) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 6, 9 )
	{
		context->addGeometry( make_AAPrism_SW( make_float3( float(1), float(i), float(2) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 7, 9 )
	{
		context->addGeometry( make_AAPrism_SW( make_float3( float(0), float(i), float(3) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 6, 9 )
	{
		context->addGeometry( make_AAPrism_SE( make_float3( float(1), float(i), float(8) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 7, 9 )
	{
		context->addGeometry( make_AAPrism_SE( make_float3( float(0), float(i), float(7) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}

	forrange( i, 3, 7 )
	{
		context->addGeometry( make_AAPrism_UW( make_float3( float(i), float(4), float(-1) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 3, 7 )
	{
		context->addGeometry( make_AAPrism_DW( make_float3( float(i), float(3), float(-1) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 3, 7 )
	{
		context->addGeometry( make_AAPrism_DW( make_float3( float(i), float(2), float(0) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 3, 7 )
	{
		context->addGeometry( make_AAPrism_DE( make_float3( float(i), float(2), float(1) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	

	forrange( i, 3, 7 )
	{
		context->addGeometry( make_AAPrism_UE( make_float3( float(i), float(4), float(11) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 3, 7 )
	{
		context->addGeometry( make_AAPrism_DE( make_float3( float(i), float(3), float(11) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 3, 7 )
	{
		context->addGeometry( make_AAPrism_DW( make_float3( float(i), float(2), float(9) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 3, 7 )
	{
		context->addGeometry( make_AAPrism_DE( make_float3( float(i), float(2), float(10) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	
	
	forrange( i, 4, 7 )
	{
		context->addGeometry( make_AAPrism_US( make_float3( float(-2), float(4), float(i) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange( i, 4, 7 )
	{
		context->addGeometry( make_AAPrism_DS( make_float3( float(-2), float(2), float(i) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange( i, 4, 7 )
	{
		context->addGeometry( make_AAPrism_DS( make_float3( float(-1), float(1), float(i) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange( i, 4, 7 )
	{
		context->addGeometry( make_AAPrism_DN( make_float3( float(1), float(1), float(i) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange( i, 4, 7 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(0), float(1), float(i) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange( i, -1, 3 )
	{
		context->addGeometry( make_AAPrism_DW( make_float3( float(i), float(2), float(3) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	forrange( i, -1, 3 )
	{
		context->addGeometry( make_AAPrism_DE( make_float3( float(i), float(2), float(7) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dbrown ) );
	}
	
	forrange( i, 3, 8 )
	{
		context->addGeometry( make_AAPrism_DN( make_float3( float(9), float(2), float(i) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 4, 7 )
	{
		context->addGeometry( make_AAPrism_DN( make_float3( float(10), float(3), float(i) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	
	
	forrange2( i, 3, 8, j, 3, 8 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(9), float(j), float(i) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange2( i, 4, 7, j, 4, 8 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(10), float(j), float(i) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 4, 9 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(8), float(i), float(1) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 5, 9 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(9), float(i), float(2) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 4, 9 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(8), float(i), float(9) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 5, 9 )
	{
		context->addGeometry( make_AAB1s( make_float3( float(9), float(i), float(8) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 4, 8 )
	{
		context->addGeometry( make_AAPrism_NW( make_float3( float(10), float(i), float(3) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	forrange( i, 4, 8 )
	{
		context->addGeometry( make_AAPrism_NE( make_float3( float(10), float(i), float(7) ), 1.f ),
							new SimpleDiffusionShaderTex( L3Dgreen ) );
	}
	
	
	running = true;
	start = control->timeMillis();
	while( running ){
		control->actions( );
		void* test = context->trace( WIDTH, HEIGHT );
		graphics->blit( test, WIDTH, HEIGHT );
		graphics->update( );
		frames++;
		free(test);
		unsigned time = control->postactions( );
		if( time < 10 )
		{
			//std::this_thread::sleep_for(std::chrono::milliseconds(10 - time));
		}
		glfwPollEvents();
		double elapsed = control->timeMillis() - start;
		if(unsigned(frames)%100==0)printf("Fps averaging to %g/s\r", (frames / elapsed)*1000.);
	}
	
	return 0;
}

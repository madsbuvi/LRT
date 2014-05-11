#include "gfx_glfw.h"


static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}


Glfwgfx::Glfwgfx( unsigned width, unsigned height )
{
	static bool engine_initialized = false;
	glfwSetErrorCallback(error_callback);
	if( !engine_initialized )
	{
		if(glfwInit()){
			engine_initialized = true;
		}
		else{
			dprintf(0, "glfw could not initialize\n");
		}
	}
	this->width = width;
	this->height = height;
	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow( width, height, title, NULL, NULL);
	if(!window)
	{
		dprintf(0, "glfw failed to create a window\n");
	}
	glfwMakeContextCurrent(window);
}

void Glfwgfx::blit( void* pixels, uint32_t width, uint32_t height )
{	
	width = width < this->width ? width : this->width;
	height = height < this->height ? height : this->height;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pixels);
}

void Glfwgfx::update( void )
{
	glfwSwapBuffers(window);
}
void Glfwgfx::delay( unsigned milliseconds )
{

}

Glfwgfx::~Glfwgfx( void )
{
	dprintf( 1, "Tearing down GLFW context\n" );
	glfwTerminate();
}
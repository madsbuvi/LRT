
#ifndef gfx_glfw_h
#define gfx_glfw_h

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <stdint.h>
#include "gfx.h"
#include "debug.h"

class Glfwgfx: public Gfx
{
	GLFWwindow* window;
	public:
	Glfwgfx( unsigned width, unsigned height );
	~Glfwgfx( void );
	void blit( void* pixels, uint32_t width, uint32_t height );
	void update( void );
	void delay( unsigned milliseconds );
	GLFWwindow* getWindow( void ){ return window; };
	
	static unsigned getTime( void )
	{
		return glfwGetTime()*1000;
	}
};

#endif
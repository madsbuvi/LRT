
#ifndef control_glfw_h
#define control_glfw_h

#include "gfx_glfw.h"
#include "common.h"
#include "control.h"
#include "clstuff.h"

class GlfwControl: public Control
{
	bool w;
	bool a;
	bool s;
	bool d;
	bool shift;
	bool ctrl;
	bool alt;
	bool lmouse;
	bool rmouse;
#ifdef USE_ROCKET
	bool processingRocket;
#endif
	int ox;
	int oy;
	int nx;
	int ny;
	
	Glfwgfx* gfx;
	public:
	void mouseCall( int button, int action, int mods );
	GlfwControl( RTContext* rtcontext, Glfwgfx* gfx
#ifdef USE_ROCKET
	,Rocket::Core::Context* rContext
#endif
	 );
	void keyboardAction( void );
	void mouseAction( void );
	void mouseMotion( void );
	void actions( void );
	unsigned postactions( void );
	unsigned timeMillis( void );
	void move( void );
};

#endif


#ifndef control_glfw_h
#define control_glfw_h

#include "gfx_glfw.h"
#include "control.h"

class GlfwControl: public Control
{
	bool w;
	bool a;
	bool s;
	bool d;
	bool shift;
	bool ctrl;
	bool lmouse;
	bool rmouse;
	int ox;
	int oy;
	int nx;
	int ny;
	
	Glfwgfx* gfx;
	
	public:
	GlfwControl( RTContext* rtcontext, Glfwgfx* gfx );
	void keyboardAction( void );
	void mouseAction( void );
	void mouseMotion( void );
	void actions( void );
	unsigned postactions( void );
	unsigned timeMillis( void );
	void move( void );
};

#endif
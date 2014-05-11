#include <vector>
#include "control_glfw.h"

GlfwControl::GlfwControl( RTContext* rtcontext, Glfwgfx* gfx )
{
	this->gfx = gfx;
	context = rtcontext;
	w = a = s = d = ctrl = shift = lmouse = rmouse = false;
}

void GlfwControl::keyboardAction( void )
{
	GLFWwindow* window = gfx->getWindow();
	
	w = glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS;
	a = glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS;
	s = glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS;
	d = glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS;
	shift = glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS;
	ctrl = glfwGetKey( window, GLFW_KEY_LEFT_CONTROL ) == GLFW_PRESS;

}

void GlfwControl::move( void )
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

void GlfwControl::mouseAction( void )
{
	GLFWwindow* window = gfx->getWindow();
	rmouse = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT ) == GLFW_PRESS;
}

void GlfwControl::mouseMotion( void )
{
	GLFWwindow* window = gfx->getWindow();
	double x,y;
	
	glfwGetCursorPos( window, &x, &y );
	nx = x;
	ny = y;
	
}

void GlfwControl::actions( void )
{
	keyboardAction();
	mouseAction();
	mouseMotion();
	move();
	time = glfwGetTime( )*1000.;
}

unsigned GlfwControl::postactions( void )
{
	return unsigned(glfwGetTime()*1000.) - time;
}

unsigned GlfwControl::timeMillis( void )
{
	return glfwGetTime( )*1000.;
}
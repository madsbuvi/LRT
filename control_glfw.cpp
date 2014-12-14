#include <vector>
#include <unordered_map>
#include "control_glfw.h"

#ifdef USE_ROCKET
#include <Rocket/Core/Input.h>
#include <Rocket/Core/Types.h>
#include <Rocket/Core/SystemInterface.h>
#endif

// Workaround for not being allowed to pass user data to glfw callbacks
// Window-Object map
static std::unordered_map<GLFWwindow*,GlfwControl*> womap;

static void mouseCallBack( GLFWwindow* widnow, int button, int action, int mods )
{
	GlfwControl* ctrl = womap[widnow];
	if(ctrl)
	{
		ctrl->mouseCall( button, action, mods );
	}
}

GlfwControl::GlfwControl( RTContext* rtcontext, Glfwgfx* gfx 
#ifdef USE_ROCKET
	,Rocket::Core::Context* rContext
#endif
)
{
	GLFWwindow* window = gfx->getWindow();
	assert(window);
	this->gfx = gfx;
	context = rtcontext;
	
	w = a = s = d = ctrl = shift = alt = lmouse = rmouse = false;
	glfwSetMouseButtonCallback( window, mouseCallBack );
	womap[gfx->getWindow()] = this;
	
	double x,y;
	glfwGetCursorPos( window, &x, &y );
	nx = ox = int(x);
	ny = oy = gfx->height - int(y);
	
#ifdef USE_ROCKET
	processingRocket = false;
	this->rContext = rContext;
#endif
}

void GlfwControl::mouseCall( int button, int action, int mods )
{
	int x,y;
	double dbx,dby;
	glfwGetCursorPos( gfx->getWindow(), &dbx, &dby );
	x = int(floor(dbx));
	// Glfw starts mouse coordinates in top-left corner, we need bottom-left.
	y = gfx->height-int(floor(dby));
	
	

#ifdef USE_ROCKET

	// If we press left inside the menu, we want the action to consider only the menu, not
	// any feature underneath (visible or not).
	// This solution is a hack since librocket has neither a function to test for a click
	// nor any return values indicating whether a click was processed.
	// The root element covers the entire surface area, thus we have to test for its ID
	// rather than just test whether the returned element is NULL. Fortunately we set the
	// ID in the context constructor, so i know precisely what to test for.
	if( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS )
	{
		processingRocket = rContext->GetHoverElement()->GetId()!="soos";
	}

	if(processingRocket)
	{
		//ensure we get rocket's value for the button
		int rButton = button == GLFW_MOUSE_BUTTON_RIGHT;
		if( action == GLFW_PRESS )
		{
			rContext->ProcessMouseButtonDown( button, 0 );
		}
		else if ( action == GLFW_RELEASE )
		{
			rContext->ProcessMouseButtonUp( button, 0 );
		}
	}
	else
{
#endif

	if( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS )
	{
		gfx->context->select( x, y, gfx->width, gfx->height );
	}
	if( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE )
	{
		gfx->context->deselect(  );
	}

#ifdef USE_ROCKET
}
	
	// Release menu
	if( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE )
	{
			processingRocket = false;
	}
#endif

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
	alt = glfwGetKey( window, GLFW_KEY_LEFT_ALT ) == GLFW_PRESS;

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
	if(dx || dy){
		if(rmouse) context->rmouse( dx, dy, ctrl, shift, alt );
		if(lmouse) context->lmouse( dx, dy, ctrl, shift, alt );
	}
}



void GlfwControl::mouseAction( void )
{
	GLFWwindow* window = gfx->getWindow();
	rmouse = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT ) == GLFW_PRESS;
	lmouse = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS;
}

void GlfwControl::mouseMotion( void )
{
	GLFWwindow* window = gfx->getWindow();
	double x,y;
	
	glfwGetCursorPos( window, &x, &y );
	nx = x;
	ny = gfx->height - int(y);
	
#ifdef USE_ROCKET
	rContext->ProcessMouseMove( x, y, 0 );
#endif
	
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












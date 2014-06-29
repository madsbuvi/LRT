
#ifndef menu_h
#define menu_h

#include "common.h"
#include "texture.h"
#include "gfx_glfw.h"

typedef void (*MCallback)( void* );


class MWidget
{
	protected:
	int2 min;
	int2 span;
	bool enabled;
	
	public:
	MWidget( int2 min, int2 max )
		:min(min), span(max - min), enabled(true) { };
		
	bool isEnabled( void ){ return enabled; };
	void enable( void ){ enabled = true; };
	void disable( void ){ enabled = false; };
	
	virtual void draw( GLFWwindow* window ) = 0;
	
	void redefine( int2 newMin, int2 newMax ){ min = newMin; span = newMax - newMin; };
};

class MWButton : public MWidget
{
	protected:
	MCallback callee;
	void* userData;
	Image* image;
	
	
	public:
	MWButton( int2 min, int2 max, MCallback callee, void* userData )
		:MWidget(min, max), callee(callee), userData(userData), image(NULL) { };
	MWButton( int2 min, int2 max, MCallback callee, void* userData, Image* image )
		:MWidget(min, max), callee(callee), userData(userData), image(image) { };
	
	void setData( void* userData){ this->userData = userData; };
	void setCallback( MCallback callee ){ this->callee = callee; };
	void setImage( Image* image ){ this->image = image; };
	
	// Calls callback function
	virtual void click( void ){ callee( userData ); };
	
	// if coord is within the button's rectangle calls callback function and returns true.
	bool attemptClick( int2 coord );
	
	// Returns true if coord is within the button's rectangle. Does NOTHING else.
	bool peek( int2 coord );
	
	//Draws the button
	virtual void draw( GLFWwindow* window ){};
	
};

class MWMenu : public MWidget
{
	protected:
	Image* image;
	std::vector<MWButton*> buttons;
	public:
	MWMenu( int2 min, int2 max )
		:MWidget(min, max), image(NULL) { };
	MWMenu( int2 min, int2 max, Image* image )
		:MWidget(min, max), image(image) { };
	virtual void draw( GLFWwindow* window );
};


class MContainer
{
	protected:
	int2 min;
	int2 span;
	bool enabled;
	Image* background;
	
	public:
	MContainer( int2 min, int2 max )
		:min(min), span(max - min), background(NULL) { };
	MContainer( int2 min, int2 max, Image* background )
		:min(min), span(max - min), background(background) { };
};







































#endif

#include <SDL.h>
#include "control.h"

class SDLControl: public Control
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
	public:
	SDLControl( RTContext* rtcontext );
	void keyboardActions( void );
	void keyboardAction( SDL_KeyboardEvent event );
	void mouseActions( void );
	void mouseAction( SDL_MouseButtonEvent event );
	void mouseMotion( SDL_MouseMotionEvent event );
	void actions( void );
	unsigned postactions( void );
	unsigned timeMillis( void );
	void move( void );
};
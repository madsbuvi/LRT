
#ifndef gfx_h
#define gfx_h

#include "clstuff.h"

class Gfx
{
	protected:
	static constexpr const char* title = "Lemmings 3D: RT";
	public:
	RTContext* context;
	unsigned width;
	unsigned height;
	virtual void blit( void* pixels, uint32_t width, uint32_t height )=0;
	virtual void update( void )=0;
	virtual void delay( unsigned milliseconds )=0;
};

#endif
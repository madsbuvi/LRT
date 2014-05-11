
#ifndef gfx_h
#define gfx_h

class Gfx
{
	protected:
	unsigned width;
	unsigned height;
	static constexpr const char* title = "Lemmings 3D: RT";
	public:
	virtual void blit( void* pixels, uint32_t width, uint32_t height )=0;
	virtual void update( void )=0;
	virtual void delay( unsigned milliseconds )=0;
};

#endif
class Gfx
{
	protected:
	unsigned wwidth;
	unsigned wheight;
	public:
	virtual void blit( void* pixels, uint32_t width, uint32_t height )=0;
	virtual void update( void )=0;
	virtual void delay( unsigned milliseconds )=0;
};
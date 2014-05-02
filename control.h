#include <stdint.h>
#include "clstuff.h"
class Control
{
	protected:
	RTContext* context;
	unsigned time;
	public:
	virtual void keyboardActions( void ) = 0;
	virtual void mouseActions( void ) = 0;
	virtual void actions( void ) = 0;
	virtual unsigned postactions( void ) = 0;
};
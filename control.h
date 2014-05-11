
#ifndef control_h
#define control_h

#include <stdint.h>
#include <iostream>
#include <stdio.h>
#include "clstuff.h"
class Control
{
	protected:
	RTContext* context;
	unsigned time;
	public:
	virtual void actions( void ) = 0;
	virtual unsigned postactions( void ) = 0;
	virtual unsigned timeMillis( void ) = 0;
};

#endif
#include <CL/cl.h>
#include <vector>
#include "debug.h"
#include "clstuff.h"
#include "geometry.h"
int main( void )
{
	int num_devices = clinit();
	if(!num_devices)
	{
		dprintf( 0, "No GPU devices available\n");
		return 0;
	}
	device_context context = create_clcontext( 0 );
}
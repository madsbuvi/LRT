#ifndef debug_h
#define debug_h

#include <cstdio>
#define dprintf(a, ...) if(a<DEBUG){fprintf( stderr, "%s.%d:", __FILE__, __LINE__ ); fprintf( stderr, __VA_ARGS__ );}

#endif
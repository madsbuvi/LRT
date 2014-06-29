#ifndef debug_h
#define debug_h

#include <cstdio>
#define dprintf(a, ...) if(a<DEBUG){fprintf( stderr, "%s.%d:", __FILE__, int(__LINE__) ); fprintf( stderr, __VA_ARGS__ );}


#define glErrorCheck glErrorCheck_( int(__LINE__), __FILE__)

void glErrorCheck_( int line, const char* file );

#endif

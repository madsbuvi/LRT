#include <GL/gl.h>
#include <GL/glu.h>
#include "debug.h"

void glErrorCheck_( int line, const char* file )
{
	GLenum errCode;
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
	   fprintf (stderr, "%s.%d: OpenGL Error: %s\n", file, line, errString);
	}
}


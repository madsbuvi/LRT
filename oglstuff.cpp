
#include "oglstuff.h"
#include <SOIL.h>

// I dropped interop but i kept this because i might decide to make various functions to load
// textures directly into specific formats later. Whatever.

unsigned char* load_OGL_texture(const char* file, int& width, int& height)
{
	return SOIL_load_image(file, &width, &height, 0, SOIL_LOAD_RGBA);
}
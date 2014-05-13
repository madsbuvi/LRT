
#ifndef texture_h
#define texture_h

#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <vector>
#include <stdint.h>

class Texture
{
	static std::vector<Texture*> textures;
	static size_t frame_width;
	static size_t frame_height;
	public:
	const char* filename;
	unsigned char* texture;
	int ID;
	size_t width;
	size_t height;
	Texture( const char* filename );
	void writeTextureData( std::vector<float>& buffer );
	static cl_mem compileTextureImage( cl_context clcontext, cl_command_queue clqueue );
};

#endif
#include <SOIL/SOIL.h>
#include "texture.h"
#include "clutil.h"
#include "debug.h"

std::vector<Texture*> Texture::textures;
size_t Texture::frame_width = 0;
size_t Texture::frame_height = 0;

unsigned char* load_texture(const char* file, int* width, int* height)
{
	return SOIL_load_image(file, width, height, 0, SOIL_LOAD_RGBA);
}

Texture::Texture( const char* filename )
{
	static int ID = 0;
	int width;
	int height;
	texture = load_texture( filename, &width, &height );
	if(!texture)
	{
		dprintf(0, "Fatal: could not load texture \"%s\"\n", filename);
	}
	this->width = width;
	this->height = height;
	this->ID = ID++;
	this->filename = filename;
	textures.push_back(this);
	frame_width = frame_width > size_t(width) ? frame_width : size_t(width);
	frame_height = frame_height > size_t(height) ? frame_height : size_t(height);
}

cl_mem Texture::compileTextureImage( cl_context clcontext, cl_command_queue clqueue )
{
	cl_mem image = 0;

	cl_image_format imf = { CL_RGBA, CL_UNSIGNED_INT8 };
	cl_image_desc imd =
	{
		CL_MEM_OBJECT_IMAGE2D_ARRAY,
		frame_width,
		frame_height,
		1,
		textures.size(),
		0,
		0,
		0,
		0,
		0
	};
	
	HandleErrorPar(
		image = clCreateImage( clcontext,
				CL_MEM_READ_ONLY,
				&imf,
				&imd,
				NULL,
				HANDLE_ERROR
			)
	);
	for( unsigned i = 0; i < textures.size(); i++ )
	{
		
		
		size_t origin[] = {0,0,i};
		size_t region[] = {
			textures[i]->width,
			textures[i]->height,
			1
		};
		HandleErrorRet( 
			clEnqueueWriteImage( clqueue,
					image,
					i == textures.size()-1,
					origin,
					region,
					textures[i]->width*4,
					0,
					textures[i]->texture,
					0,
					NULL,
					NULL
				)
			);
	}
	
	return image;
}

void Texture::writeTextureData( std::vector<float>& buffer )
{
	buffer.push_back( float(ID) );
	buffer.push_back( float(width)/float(frame_width) );
	buffer.push_back( float(height)/float(frame_height) );
}

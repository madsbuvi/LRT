#include "shaders.h"

std::vector<Shader*> Shader::shaders;
Shader* Shader::getDefaultShader( void )
{ 
		if( shaders.size() == 0 )
		{
			throw std::runtime_error( "Attempted to run raytracer without defining any shaders.\n" );
		}
		return shaders[0];
}

void SimpleDiffusionShader::writeShaderData( std::vector<float>& buffer )
{
	buffer.push_back( color.x );
	buffer.push_back( color.y );
	buffer.push_back( color.z );
}
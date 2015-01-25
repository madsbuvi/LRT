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


void Shader::writeShaders( ostream& out )
{
	for( Shader* boop: shaders )
	{
		boop->writeOff( out );
		out << std::endl;
	}
}

void Shader::clear( void )
{
	for( Shader* boop: shaders)
	{
		delete boop;
	}
	shaders.clear();
}

void SimpleDiffusionShader::writeOff( ostream& out )
{
	out << "SIMPLEDIFFUSIONSHADER{" << ID << "}{ " << m_color << " }";
}

void SimpleDiffusionShader::writeShaderData( std::vector<float>& buffer )
{
	buffer.push_back( m_color.x );
	buffer.push_back( m_color.y );
	buffer.push_back( m_color.z );
}

void SimpleDiffusionShaderTex::writeOff( ostream& out )
{
	out << "SIMPLEDIFFUSIONSHADERTEX{" << ID << "}{ " << "\"" << std::string(m_filename) << "\"" << " }";
}

void SimpleDiffusionShaderTex::writeShaderData( std::vector<float>& buffer )
{
	m_tex->writeTextureData( buffer );
}

SimpleDiffusionShaderTex::~SimpleDiffusionShaderTex( void )
{
}
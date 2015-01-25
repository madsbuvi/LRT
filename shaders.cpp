#include "shaders.h"

void ShaderContext::writeShaders( ostream& out )
{
	for( Shader* boop: m_Shaders )
	{
		boop->writeOff( out );
		out << std::endl;
	}
}

void ShaderContext::clear( void )
{
	for( Shader* boop: m_Shaders)
	{
		delete boop;
	}
	m_Shaders.clear();
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

void ShaderContext::writeShaderData( std::vector<float>& buffer )
{
	for( Shader* boop: m_Shaders )
	{
		boop->m_DataIndex = buffer.size();
		boop->writeShaderData( buffer );
	}
}

ShaderContext::~ShaderContext( void )
{
	for( Shader* boop: m_Shaders )
	{
		delete boop;
	}
}

void	ShaderContext::addShader( Shader* shader )
{
	shader->ID = m_Shaders.size();
	m_Shaders.push_back( shader );
}


Shader*	ShaderContext::makeSimpleDiffusionShaderTex( const char* filename )
{
	Shader* shader = new SimpleDiffusionShaderTex( filename );
	addShader( shader );
	return shader;
}

Shader*	ShaderContext::makeSimpleDiffusionShader( float3 color )
{
	Shader* shader = new SimpleDiffusionShader( color );
	addShader( shader );
	return shader;
}
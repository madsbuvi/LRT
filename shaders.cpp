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
	int dataIndex = 0;
	for( Shader* boop: m_Shaders )
	{
		boop->writeShaderData( buffer );
		boop->m_DataIndex = dataIndex;
		dataIndex++;
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
#ifndef shaders_h
#define shaders_h

#include "vtypes.h"
#include "debug.h"
#include <vector>
#include <stdexcept>
#include "texture.h"
#include <stdio.h>
#include <iostream>
using std::ostream;

#define SIMPLE_DIFFUSION_SHADER 0
#define SIMPLE_DIFFUSION_SHADER_TEX 1


class Shader
{
	
	static std::vector<Shader*> shaders;
	
	public:
	static Shader* 	getDefaultShader( void );
	static void 	writeShaders( ostream& out );
	static void 	clear( void );
	
	virtual void writeOff( ostream& out ) = 0;
	virtual void writeShaderData( std::vector<float>& buffer ) = 0;
	int getID( void ) { return ID; };
	int shader;
	
	virtual ~Shader( void ){};
	
	
	protected:
	int ID;
	Shader(void){ 
		ID = shaders.size();
		shaders.push_back(this);
	};
	
	
};

class SimpleDiffusionShader: public Shader
{
	float3 m_color;
	public:
	SimpleDiffusionShader( float3 color ){
		shader = SIMPLE_DIFFUSION_SHADER;
		m_color = color;
	};
	virtual ~SimpleDiffusionShader( void ){};
	void writeShaderData( std::vector<float>& buffer );
	void writeOff( ostream& out );
};


class SimpleDiffusionShaderTex: public Shader
{
	Texture* 	m_tex;
	const char* m_filename;
	public:
	SimpleDiffusionShaderTex( const char* filename ){
		shader = SIMPLE_DIFFUSION_SHADER_TEX;
		m_tex = new Texture( filename );
		m_filename = filename;
	};
	virtual ~SimpleDiffusionShaderTex( void );
	void writeShaderData( std::vector<float>& buffer );
	void writeOff( ostream& out );
};


#endif
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
	friend class ShaderContext;
	
	//! Index into shaderdata vector
	int m_DataIndex;
	
	public:
	//! Return the unique ID of this shader (equivalent to its index in the shader context's shader vector)
	int getID( void ) { return ID; };
	
	// Numeric identifier of shader type
	int shader;
	
	// Generic destructor
	virtual ~Shader( void ){};
	
	
	protected:
	//! Shader ID
	int ID;
	
	//! Write shader to text
	virtual void writeOff( ostream& out ) = 0;
	
	//! Write shader to float vector
	virtual void writeShaderData( std::vector<float>& buffer ) = 0;
	
	
};

class ShaderContext
{
	//! Vector of the shaders related to this context
	std::vector<Shader*> m_Shaders;
	
	//! The default shader
	Shader* m_DefaultShader;
	
	
	public:
	//! Returns the default shader of this context
	Shader* getDefaultShader( void ){ return m_DefaultShader; };
	
	//! Sets the default shader of this context
	void 	setDefaultShader( Shader* defaultShader){ m_DefaultShader = defaultShader; };
	
	//! Write all shader data to float vector
	void 	writeShaderData( std::vector<float>& buffer );
	
	//! Write all shaders to text
	void 	writeShaders( ostream& out );
	
	//! Wipe all shader data (Not recommended, just delete this and make a new context)
	void 	clear( void );
	
	//! Add a shader to the context
	void	addShader( Shader* shader );
	
	//! Generic constructor
			ShaderContext( void ){};
	
	//! Destructor. Will delete all shaders related to this context.
			~ShaderContext( void );

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
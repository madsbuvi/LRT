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
	
	//! Returns a numeric identifier of the shader type
	int getShaderType( void ){ return shader; };
	
	//! Returns a numeric identifier of the shader type
	int getShaderDataIndex( void ){ return m_DataIndex; };
	
	// Generic destructor
	virtual ~Shader( void ){};
	
	
	protected:
	// Numeric identifier of shader type
	int shader;
	
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
			
	//! Shader constructor wrappers. Limiting construction of shaders to these is to ensure shaders belong to this context
	Shader*	makeSimpleDiffusionShader( float3 color );
	Shader*	makeSimpleDiffusionShaderTex( const char* filename );

};

class SimpleDiffusionShader: public Shader
{
	friend class ShaderContext;
	
	float3 m_color;
	
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
	friend class ShaderContext;
	
	Texture* 	m_tex;
	const char* m_filename;
	
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
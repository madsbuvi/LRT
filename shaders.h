#ifndef shaders_h
#define shaders_h

#include "vtypes.h"
#include "debug.h"
#include <vector>
#include <stdexcept>
#include "texture.h"

#define SIMPLE_DIFFUSION_SHADER 0
#define SIMPLE_DIFFUSION_SHADER_TEX 1


class Shader
{
	
	static std::vector<Shader*> shaders;
	
	public:
	static Shader* getDefaultShader( void );
	virtual void writeShaderData( std::vector<float>& buffer ) = 0;
	int shader;
	// Note that the base Shader constructor implies that the first created shader will be the default shader until this function is called.
	// Note also that there is no destructor support yet, so all shaders should be passed by reference / pointers for now, lest the list point to dead shaders. Do not declare on stack / 10.
	void setAsDefaultShader( void ){ shaders.insert( shaders.begin(), this ); };
	
	protected:
	Shader(void){ shaders.push_back(this); };
	
	
};

class SimpleDiffusionShader: public Shader
{
	float3 color;
	public:
	SimpleDiffusionShader( float3 color ){ shader = SIMPLE_DIFFUSION_SHADER; this->color = color; };
	void writeShaderData( std::vector<float>& buffer );
};


class SimpleDiffusionShaderTex: public Shader
{
	Texture* tex;
	public:
	SimpleDiffusionShaderTex( Texture* texture ){ shader = SIMPLE_DIFFUSION_SHADER_TEX; tex = texture; };
	void writeShaderData( std::vector<float>& buffer );
};


#endif
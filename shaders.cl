
__constant const sampler_t smplr = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_LINEAR;

static Result diffusion_shader( Ray ray, float3 light_pos, Result data, __global float* shader_data )
{
	float3 hit_point = ray.origin + ray.direction * data.t;
	// Diffuse lighting based on rotation relative to light source
	float diffuseFactor = max( dot( normalize( data.normal ),
											normalize( light_pos - hit_point ) )
								, 0.0f );
	float3 totallight = diffuseFactor * 0.5f + make_float3(0.5f, 0.5f, 0.5f);
	data.result = make_float3( shader_data[0], shader_data[1], shader_data[2] ) * totallight;

	data.recur = 0;
	
	return data;
}

// Turn into constant memory to be set from host later
// Or #define it from host for extra performance

#define ambient_effect make_float3( 1.f, 1.f, 1.f )
#define ambient_color make_float3( 0.4f, 0.4f, 0.4f )
#define specular_color make_float3( 0.6f, 0.6f, 0.6f )
#define diffuse_color make_float3( 0.2f, 0.7f, 0.1f )
#define phong_exponent 132.f

static Result diffusion_shader_tex( Ray ray, float3 light_pos, Result data, __global float* shader_data,
							__read_only image2d_array_t textr )
{
	float3 hit_point = ray.origin + ray.direction * data.t;
	float3 light_dir = normalize( light_pos - hit_point );
	// Diffuse lighting based on rotation relative to light source
	float diffuseFactor = max( dot( normalize( data.normal ),
											light_dir )
								, 0.0f );
	float3 totallight = diffuseFactor * 0.5f;
	//int2 coord = { hit_point.x, hit_point };
	//printf( "<%g, %g, %g>\n",  hit_point.z, fmod(hit_point.z, shader_data[2]), gmod(hit_point.z, shader_data[2]) );
	uint4 texui = read_imageui( textr, smplr, make_float4(
							gmod(data.tex.x, shader_data[1]),
							gmod(data.tex.y, shader_data[2]),
							shader_data[0], 0 ) );
	float3 tex = make_float3( texui.x, texui.y, texui.z ) / 255.f;
	
	//float3 result = ambient_effect * ambient_color + make_float3( shader_data[0], shader_data[1], shader_data[2] ) * totallight;
	data.result = ( ambient_effect * ambient_color + totallight ) * tex;
	
	// Add shadows here
	
	// Add reflection here
	
	// Phong specular
	float3 h = normalize( light_dir - ray.direction );
	float d = dot( data.normal, h );
	float power;
	if( d > 0)
	{
		power = pow( d, phong_exponent );
		data.result += specular_color * power;
	}
	
	data.recur = 0;
	
	return data;
}

static Result phong_shader1( rt_context context, Result data,
							__read_only image2d_array_t textr )
{
	Ray ray = context.ray;
	float3 hit_point = ray.origin + ray.direction * data.t;
	float3 light_dir = normalize( context.light_pos - hit_point );
	// Diffuse lighting based on rotation relative to light source
	float diffuseFactor = max( dot( normalize( data.normal ),
											light_dir )
								, 0.0f );
	float3 totallight = diffuseFactor * 0.5f;
	//int2 coord = { hit_point.x, hit_point };
	//printf( "<%g, %g, %g>\n",  hit_point.z, fmod(hit_point.z, shader_data[2]), gmod(hit_point.z, shader_data[2]) );
	uint4 texui = read_imageui( textr, smplr, make_float4(
							gmod(data.tex.x, context.shader_data[1]),
							gmod(data.tex.y, context.shader_data[2]),
							context.shader_data[0], 0 ) );
	float3 tex = make_float3( texui.x, texui.y, texui.z ) / 255.f;
	
	// Shadows
	Ray shadow_ray = make_ray( hit_point, light_dir, REGULAR, RT_DEFAULT_MIN, RT_DEFAULT_MAX, 1.f );
	Result shadow = MISS;
	if(shadow.t > RT_DEFAULT_MIN && shadow.t < RT_DEFAULT_MAX )
	{
		totallight = 0.f;
	}
	
	//float3 result = ambient_effect * ambient_color + make_float3( shader_data[0], shader_data[1], shader_data[2] ) * totallight;
	data.result = ( ambient_effect * ambient_color + totallight ) * tex;
	
	// Add reflection here
	
	// Phong specular
	float3 h = normalize( light_dir - ray.direction );
	float d = dot( data.normal, h );
	float power;
	if( d > 0)
	{
		power = pow( d, phong_exponent );
		data.result += specular_color * power;
	}
	if( ray.relevance > 0.1 )
	{
		float amplitude = length( data.result ) / 1.732050808f;
		data.recur = 1;
		data.reflect.direction = reflect( ray.direction, data.normal );
		data.reflect.origin = hit_point;
		data.reflect.relevance = ray.relevance * 0.5 * amplitude;
	}
	else
	{
		data.recur = 0;
	}
	
	return data;
}

static Result phong_shader2( rt_context context, Result data, Result reflect, 
							__read_only image2d_array_t textr )
{
	//TODO: Make use of reflectance
	data.result = data.result + data.reflect.relevance*reflect.result;
	return data;
}

#define DEVICE_SIDE

#define REGULAR 0

#define SIMPLE_DIFFUSION_SHADER 0
#define SIMPLE_DIFFUSION_SHADER_TEX 1

#define printg3(a) if( gid() == 0 )printf( #a ": %g, %g, %g\n", a.x, a.y, a.z );
#define printg(a) if( gid() == 0 )printf( #a ": %g\n", a );
#define printi(a) if( gid() == 0 )printf( #a ": %d\n", a );
#define printu(a) if( gid() == 0 )printf( #a ": %u\n", a );
#define printg3all(a) printf( #a ": %g, %g, %g\n", a.x, a.y, a.z );
#define printgall(a) printf( #a ": %g\n", a );
#define printiall(a) printf( #a ": %d\n", a );
#define printuall(a) printf( #a ": %u\n", a );


__constant const sampler_t smplr = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_LINEAR;

enum PrimitiveType { Sphere_t, Triangle_t, Rectangle_t, Quadrilateral_t, AAB_t, Box_t };

static inline unsigned gid( void )
{
	return get_global_id(1) * get_global_size(0) + get_global_id(0);
}


#include "shared.h"


inline float fminf(const float3 a)
{
 return fmin(fmin(a.x, a.y), a.z);
}


inline float fmaxf(const float3 a)
{
 return fmax(fmax(a.x, a.y), a.z);
}

static inline float3 fminf3(const float3 a, const float3 b)
{
	return make_float3(fmin(a.x,b.x), fmin(a.y,b.y), fmin(a.z,b.z));
}

static inline float3 fmaxf3(const float3 a, const float3 b)
{
	return make_float3(fmax(a.x,b.x), fmax(a.y,b.y), fmax(a.z,b.z));
}

static uint make_color( float3 in )
{
	uint color = 0;
	color += (uint)( fmin( fabs(in.x), 1.f )*255.f );
	color = color << 8u;
	color += (uint)( fmin( fabs(in.y), 1.f )*255.f );
	color = color << 8u;
	color |= (uint)( fmin( fabs(in.z), 1.f )*255.f );
	color = color << 8u;
	color += 255u;
	return color;
}

#include "intersectors.inc"

// Returns the negative of n if i and n point in the same direction (are within 90 degrees of eachother)
// otherwise the positive of n
// This is useful to guarantee that a normal points OUT of the ground, without worrying about order of cross products or vertexes ktp.
float3 faceback(const float3 n, const float3 i)
{
  if( dot ( n, i ) < 0.f )
	return n;
  else
	return -n;
}

static float3 diffusion_shader( Ray ray, float3 light_pos, Result data, __global float* shader_data )
{
	float3 hit_point = ray.origin + ray.direction * data.t;
	// Diffuse lighting based on rotation relative to light source
	float diffuseFactor = max( dot( normalize( data.normal ),
											normalize( light_pos - hit_point ) )
								, 0.0f );
	float3 totallight = diffuseFactor * 0.5f + make_float3(0.5f, 0.5f, 0.5f);
	float3 result = make_float3( shader_data[0], shader_data[1], shader_data[2] ) * totallight;

	
	return result;
}

// Turn into constant memory to be set from host later
// Or #define it from host for extra performance

#define ambient_effect make_float3( 1.f, 1.f, 1.f )
#define ambient_color make_float3( 0.4f, 0.4f, 0.4f )
#define specular_color make_float3( 0.6f, 0.6f, 0.6f )
#define diffuse_color make_float3( 0.2f, 0.7f, 0.1f )
#define phong_exponent 132.f

static float gmod( float a1, float a2 )
{
	float ratio = a1 / a2;
	a1 = (ratio - floor(ratio))*a2;
	if(a2 - a1 < 5e-5) return 0.f;
	return a1;
}

static float3 diffusion_shader_tex( Ray ray, float3 light_pos, Result data, __global float* shader_data,
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
	float3 result = ( ambient_effect * ambient_color + totallight ) * tex;
	
	// Add shadows here
	
	// Add reflection here
	
	// Phong specular
	float3 h = normalize( light_dir - ray.direction );
	float d = dot( data.normal, h );
	float power;
	if( d > 0)
	{
		power = pow( d, phong_exponent );
		result += specular_color * power;
	}
	
	
	return result;
}



/* Raytracer kernel.
 *
 * Has an ungodly number of parameters because opencl does not have any proper way of ensuring consistency in layout of
 * structures between host and device, so to ensure portability i can only use very basic types. So every struct becomes a set of arrays.
 */
__kernel void raytrace( float3 U, float3 V, float3 W, float3 eye, __global uint* picture,
	
	/* All our basic primitives, which will serve to build all other shapes */
	/* Sphere parameters */
	__global float3* sphere_centers, __global float* sphere_radi, int nspheres,
	/* Triangle parameters */
	__global float3* triangles, int ntriangles,
	/* Box parameters */
	__global float3* AABs, int nAABs,
	/* Box parameters */
	__global float3* boxes, __global float* box_heights, int nboxes,
	
	/* Geometry */
	__global int* geometry, __global int* primitives, int nobjects,
	
	/* Shader stuff */
	float3 light_pos,
	
	/* Shader data */
	__global float* shader_data,
	
	/* Texture(s) */
	__read_only image2d_array_t pack
	)
{
	// CAMERA
	float2 d1 = {get_global_id(0), get_global_id(1)};
	float2 d2 = {get_global_size(0), get_global_size(1)};
	float2 d = d1 / d2 * 2.f - 1.f;
	float3 ray_origin = eye;
	float3 ray_direction = normalize(d.x*U - d.y*V + W);
	/*
	if( fabs(ray_direction.x) < RT_MIN_ANGLE ) ray_direction.x = RT_MIN_ANGLE;
	if( fabs(ray_direction.y) < RT_MIN_ANGLE ) ray_direction.y = RT_MIN_ANGLE;
	if( fabs(ray_direction.z) < RT_MIN_ANGLE ) ray_direction.z = RT_MIN_ANGLE;
	*/
	Ray ray = make_ray(ray_origin, ray_direction, REGULAR, RT_DEFAULT_MIN, RT_DEFAULT_MAX);
	
	
	// RAY TRACING
	
	Result closest = MISS;
	closest.t = RT_DEFAULT_MAX+1.f;
	Result nyligst;
	int shader_i = 0;
	//printi( nobjects );
	
	for( int i = 0; i < nobjects; i++ )
	{
		__global int* p = &primitives[ geometry[ i*4 ] ];
		int nprim = geometry[ i*4 + 1];
		for( int j = 0; j < nprim; j++ )
		{
			int type = p[ j*2 ];
			int index = p[ j*2 + 1 ];
			
			
			switch( type )
			{
				case Sphere_t:
				{
					Sphere sphere = { sphere_centers[index], sphere_radi[index] };
					nyligst = intersect_sphere( ray, sphere );
					break;
				}	
				case Triangle_t:
					nyligst = intersect_triangle( ray, triangles[index*3], triangles[index*3+1], triangles[index*3+2] );
					break;
					
				case AAB_t:
					nyligst = intersect_AAB( ray, AABs[ index*2 ], AABs[ index*2 + 1 ] );
					break;
				case Box_t:
				{
					Box box = {
						boxes[ index*3 + 0 ],
						boxes[ index*3 + 1 ],
						boxes[ index*3 + 2 ],
						box_heights[ index ]
					};

					nyligst = intersect_box( ray, box );
					
					break;
				}
			}

			if( nyligst.t > RT_DEFAULT_MIN && nyligst.t < closest.t )
			{
				closest = nyligst;
				shader_i = i;
			}
			
		}
	}
	int shader = geometry[ shader_i*4 + 2 ];
	int shader_data_index = geometry[ shader_i*4 + 3 ];

	
	float3 result = make_float3( 0.f, 0.f, 0.f );
	if(closest.t > RT_DEFAULT_MIN && closest.t < RT_DEFAULT_MAX )
	{
		closest.normal = faceback( closest.normal, ray.direction );
		switch( shader )
		{
			case SIMPLE_DIFFUSION_SHADER:
				result = diffusion_shader( ray, light_pos, closest, &shader_data[ shader_data_index ]);
				break;
			case SIMPLE_DIFFUSION_SHADER_TEX:
				result = diffusion_shader_tex( ray, light_pos, closest, &shader_data[ shader_data_index ], pack );
				break;
		}
	}
	picture[ gid( ) ] = make_color( result );
	
}

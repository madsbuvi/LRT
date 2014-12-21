
#define DEVICE_SIDE

#define REGULAR 0

#define SIMPLE_DIFFUSION_SHADER 2
#define SIMPLE_DIFFUSION_SHADER_TEX 3
#define SIMPLE_DIFFUSION_SHADER_REFLECT 0
#define SIMPLE_DIFFUSION_SHADER_TEX_REFLECT 1

#define printg3(a) if( gid() == 0 )printf( #a ": %g, %g, %g\n", a.x, a.y, a.z );
#define printg(a) if( gid() == 0 )printf( #a ": %g\n", a );
#define printi(a) if( gid() == 0 )printf( #a ": %d\n", a );
#define printu(a) if( gid() == 0 )printf( #a ": %u\n", a );
#define printg3all(a) printf( #a ": %g, %g, %g\n", a.x, a.y, a.z );
#define printgall(a) printf( #a ": %g\n", a );
#define printiall(a) printf( #a ": %d\n", a );
#define printuall(a) printf( #a ": %u\n", a );

//#define ifprint(...) if( get_global_id(0) == 160 && get_global_id(1) == 120 ){ printf(__VA_ARGS__);}
#define ifprint(...) 


__constant const sampler_t smplr = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_LINEAR;

enum PrimitiveType { Sphere_t, Triangle_t, Rectangle_t, Quadrilateral_t, AAB_t, Box_t };



static inline unsigned gid( void )
{
	return get_global_id(1) * get_global_size(0) + get_global_id(0);
}


#include "shared.h"

typedef struct {
	/* All our basic primitives, which will serve to build all other shapes */
	/* Sphere parameters */
	__global float3* sphere_centers;
	__global float* sphere_radi;
	int nspheres;
	/* Triangle parameters */
	__global float3* triangles;
	int ntriangles;
	/* Axis-Aligned Box parameters */
	__global float3* AABs;
	int nAABs;
	/* Box parameters */
	__global float3* boxes;
	__global float* box_heights;
	int nboxes;
	
	/* Geometry */
	__global int* geometry;
	__global int* primitives;
	int nobjects;
	
	/* Shader data */
	float3 light_pos;
	__global float* shader_data;
	
	/* BVH data */
	
	__global int* bvh_ints;
	__global float* bvh_floats;
	
	/* Active ray */
	Ray ray;
} rt_context;


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

static float3 reflect(float3 v, float3 n)
{
	float3 reflect = n;
	float d = 2.f*dot(v, n);
	reflect *= d;
	reflect /= dot(n, n);
	
	return v - reflect;
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

static float gmod( float a1, float a2 )
{
	float ratio = a1 / a2;
	a1 = (ratio - floor(ratio))*a2;
	if(a2 - a1 < 5e-5) return 0.f;
	return a1;
}

static Result intersect(
	rt_context context,
	Ray ray
	)
{
	// RAY TRACING
	Result closest = MISS;
	closest.t = RT_DEFAULT_MAX+1.f;
	Result nyligst;
	//printi( nobjects );
	
	for( int i = 0; i < context.nobjects; i++ )
	{
		__global int* p = &context.primitives[ context.geometry[ i*4 ] ];
		int nprim = context.geometry[ i*4 + 1];
		for( int j = 0; j < nprim; j++ )
		{
			int type = p[ j*2 ];
			int index = p[ j*2 + 1 ];
			
			
			switch( type )
			{
				case Sphere_t:
				{
					Sphere sphere = { context.sphere_centers[index], context.sphere_radi[index] };
					nyligst = intersect_sphere( ray, sphere );
					break;
				}	
				case Triangle_t:
					nyligst = intersect_triangle( ray, context.triangles[index*3], context.triangles[index*3+1], context.triangles[index*3+2] );
					break;
					
				case AAB_t:
					nyligst = intersect_AAB( ray, context.AABs[ index*2 ], context.AABs[ index*2 + 1 ] );
					break;
				case Box_t:
				{
					Box box = {
						context.boxes[ index*3 + 0 ],
						context.boxes[ index*3 + 1 ],
						context.boxes[ index*3 + 2 ],
						context.box_heights[ index ]
					};

					nyligst = intersect_box( ray, box );
					
					break;
				}
			}

			if( nyligst.t > RT_DEFAULT_MIN && nyligst.t < closest.t )
			{
				closest = nyligst;
				closest.shader = i;
				ifprint("REGULAR New closest at <%d, %d, %d>\n", type, index, i);
			}
			
		}
	}
	return closest;
}

static Result bvhintersect(
	rt_context context,
	Ray ray
	)
{
	// RAY TRACING
	Result closest = MISS;
	closest.t = RT_DEFAULT_MAX+1.f;
	Result nyligst;
	//printi( nobjects );
	
	int id = 0;
	
	while( id >= 0 )
	{
		ifprint("\nEntering new id[%d]\n", id);
		int type = context.bvh_ints[ id*6 + 0 ];
		int index = context.bvh_ints[ id*6 + 1 ];
		int shader = context.bvh_ints[ id*6 + 2 ];
		int shaderIndex = context.bvh_ints[ id*6 + 3 ];
		int next = context.bvh_ints[ id*6 + 4 ];
		int failureLink = context.bvh_ints[ id*6 + 5 ];
		
		
		ifprint("type: %d\n", type);
		ifprint("index: %d\n", index);
		ifprint("shader: %d\n", shader);
		ifprint("shaderIndex: %d\n", shaderIndex);
		ifprint("next: %d\n", next);
		ifprint("failureLink: %d\n", failureLink);
		
		float t = bound(
			ray,
			make_float3( context.bvh_floats[ id*6 + 0 ], context.bvh_floats[ id*6 + 1 ], context.bvh_floats[ id*6 + 2 ] ),
			make_float3( context.bvh_floats[ id*6 + 3 ], context.bvh_floats[ id*6 + 4 ], context.bvh_floats[ id*6 + 5 ] )
			);
			
		float3 bmin = make_float3( context.bvh_floats[ id*6 + 0 ], context.bvh_floats[ id*6 + 1 ], context.bvh_floats[ id*6 + 2 ] );
		float3 bmax = make_float3( context.bvh_floats[ id*6 + 3 ], context.bvh_floats[ id*6 + 4 ], context.bvh_floats[ id*6 + 5 ] );
		ifprint("bmin: <%.4g, %.4g, %.4g>\n", bmin.x, bmin.y, bmin.z);
		ifprint("bmax: <%.4g, %.4g, %.4g>\n", bmax.x, bmax.y, bmax.z);
		ifprint("t: %g\n", t);
		if( t > RT_DEFAULT_MIN && t < closest.t )
		{
			// Node is relevant
			
			if( type >= 0 )
			{
				// Leaf node
				// Test the primitive
				switch( type )
				{
					case Sphere_t:
					{
						Sphere sphere = { context.sphere_centers[index], context.sphere_radi[index] };
						nyligst = intersect_sphere( ray, sphere );
						break;
					}	
					case Triangle_t:
						nyligst = intersect_triangle( ray, context.triangles[index*3], context.triangles[index*3+1], context.triangles[index*3+2] );
						break;
						
					case AAB_t:
						nyligst = intersect_AAB( ray, context.AABs[ index*2 ], context.AABs[ index*2 + 1 ] );
						break;
					case Box_t:
					{
						Box box = {
							context.boxes[ index*3 + 0 ],
							context.boxes[ index*3 + 1 ],
							context.boxes[ index*3 + 2 ],
							context.box_heights[ index ]
						};

						nyligst = intersect_box( ray, box );
						
						break;
					}
				}

				if( nyligst.t > RT_DEFAULT_MIN && nyligst.t < closest.t )
				{
					closest = nyligst;
					closest.shader = shader;
					ifprint("New closest at <%d, %d, %d>\n", type, index, shader);
				}
				
				id = next;
			}
			else
			{
				// Internal node, continue exploring the tree
				id = next;
			}
		}
		else
		{
			//Follow failure link
			id = failureLink;
		}
		
	}
	return closest;
}

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
		data.recur = 1;
		data.reflect.direction = reflect( ray.direction, data.normal );
		data.reflect.origin = hit_point;
		data.reflect.relevance = ray.relevance * 0.33;
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
	/* Axis-Aligned Box parameters */
	__global float3* AABs, int nAABs,
	/* Box parameters */
	__global float3* boxes, __global float* box_heights, int nboxes,
	
	/* Geometry */
	__global int* geometry, __global int* primitives, int nobjects,
	
	/* Shader data */
	float3 light_pos,
	__global float* shader_data,
	
	/* BVH data */
	
	__global int* bvh_ints,
	__global float* bvh_floats,
	
	/* Texture(s) */
	__read_only image2d_array_t pack
	)
{
	// CAMERA
	float2 d1 = {get_global_id(0), get_global_id(1)};
	float2 d2 = {get_global_size(0), get_global_size(1)};
	

	ifprint("STARTING NEW TRACE: %d, %d\n", get_global_id(0), get_global_id(1));
	
	float2 d = d1 / d2 * 2.f - 1.f;
	float3 ray_origin = eye;
	float3 ray_direction = normalize(d.x*U - d.y*V + W);
	
	int lid = get_local_id(0) * get_local_size(0) + get_local_id(1);
	
	rt_context context = 
	{
		sphere_centers, sphere_radi, nspheres,
		triangles, ntriangles,
		AABs, nAABs,
		boxes, box_heights, nboxes,
		geometry, primitives, nobjects,
		light_pos, shader_data,
		bvh_ints, bvh_floats
	};
	
	/*
	if( fabs(ray_direction.x) < RT_MIN_ANGLE ) ray_direction.x = RT_MIN_ANGLE;
	if( fabs(ray_direction.y) < RT_MIN_ANGLE ) ray_direction.y = RT_MIN_ANGLE;
	if( fabs(ray_direction.z) < RT_MIN_ANGLE ) ray_direction.z = RT_MIN_ANGLE;
	*/
	Ray ray = make_ray(ray_origin, ray_direction, REGULAR, RT_DEFAULT_MIN, RT_DEFAULT_MAX, 1.f);
		
	unsigned indice = 1;
	
	#define STACK_SIZE 5
	
	Result result_stack[STACK_SIZE];
	Ray ray_stack[STACK_SIZE] = {ray};
	float3 result = make_float3( 0.f, 0.f, 0.f );
	
	result_stack[0].trace = 1;
	
	while( indice )
	{
		context.ray = ray_stack[indice - 1];
		Result active = result_stack[indice - 1];
		ifprint("\n\n\nindice[%u]:\n", indice);
		
		if(active.trace)
		{
			active = bvhintersect( context, context.ray );
			ifprint("intersection result:\nnormal: <%.4g, %.4g, %.4g>\n", active.normal.x, active.normal.y, active.normal.z);
			ifprint("intersection result:\nt: <%.4g>\n", active.t);
			ifprint("intersection result:\ntex: <%.4g, %.4g>\n", active.tex.x, active.tex.y);
			ifprint("intersection result:\nshader: <%u>\n", active.shader);
			int shader = geometry[ active.shader*4 + 2 ];
			int shader_data_index = geometry[ active.shader*4 + 3 ];
			context.shader_data = &shader_data[ shader_data_index ];
				
			if(active.t > RT_DEFAULT_MIN && active.t < RT_DEFAULT_MAX )
			{
				active.normal = faceback( active.normal, ray.direction );
				switch( shader )
				{
					//case SIMPLE_DIFFUSION_SHADER:
					//	active = diffusion_shader( ray, light_pos, active, &shader_data[ shader_data_index ]);
					//	break;
					//case SIMPLE_DIFFUSION_SHADER_TEX:
					//	active = diffusion_shader_tex( ray, light_pos, active, &shader_data[ shader_data_index ], pack );
					//	break;
					case SIMPLE_DIFFUSION_SHADER_TEX_REFLECT:
						active = phong_shader1( context, active, pack );
						break;
				}
			}
		}
		else if (active.recur)
		{
			active.recur = 0;
			int shader = geometry[ active.shader*4 + 2 ];
			int shader_data_index = geometry[ active.shader*4 + 3 ];

			switch( shader )
			{
				case SIMPLE_DIFFUSION_SHADER_TEX_REFLECT:
					active = phong_shader2( context, active, result_stack[indice], pack );
					break;
			}
		}
		
		active.trace = 0;
		unsigned nindice = indice;
		if( active.recur && indice < STACK_SIZE )
		{
			nindice++;
			result_stack[nindice-1].trace = 1;
			ray_stack[nindice-1] = active.reflect;
		}
		else
		{
			nindice--;
		}
		result_stack[indice - 1] = active;
		indice = nindice;

	}
	
	picture[ gid( ) ] = make_color( result_stack[0].result );
	
}

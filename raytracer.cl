
#define DEVICE_SIDE

#define REGULAR 0

#define SIMPLE_DIFFUSION_SHADER 2
#define SIMPLE_DIFFUSION_SHADER_TEX 3
#define SIMPLE_DIFFUSION_SHADER_REFLECT 0
#define SIMPLE_DIFFUSION_SHADER_TEX_REFLECT 1


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

#include "utilities.cl"
#include "intersectors.inc"
#include "intersect.cl"
#include "shaders.cl"


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

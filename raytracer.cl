#define REGULAR 0
#define RT_DEFAULT_MIN 0.001
#define RT_DEFAULT_MAX 10000.

typedef struct{
	float3 origin;
	float3 direction;
	int type;
	float min;
	float max;
} Ray;

static Ray make_ray( float3 o, float3 dir, int type, float min, float max )
{
	Ray ray = {o, dir, type, min, max};
	return ray;
}

static uint make_color( float3 in )
{
	uint color = 255u;
	color = color << 8u;
	color += fabs(in.x)*255u;
	color = color << 8u;
	color += fabs(in.y)*255u;
	color = color << 8u;
	color += fabs(in.z)*255u;
	return color;
}

static float3 trace(Ray ray)
{
	float3 asdf = {0.f, 1.f, 0.f};
	return asdf;
}

static void pinhole( float3 U, float3 V, float3 W, float3 eye, __global uint* picture )
{
	
	float2 d1 = {get_global_id(0), get_global_id(1)};
	float2 d2 = {get_global_size(0), get_global_size(1)};
	float2 d = d1 / d2 * 2.f - 1.f;
	float3 ray_origin = eye;
	float3 ray_direction = normalize(d.x*U + d.y*V + W);
	/*
	if( fabs(ray_direction.x) < RT_MIN_ANGLE) ray_direction.x = RT_MIN_ANGLE;
	if( fabs(ray_direction.y) < RT_MIN_ANGLE ) ray_direction.y = RT_MIN_ANGLE;
	if( fabs(ray_direction.z) < RT_MIN_ANGLE ) ray_direction.z = RT_MIN_ANGLE;
	*/
	Ray ray = make_ray(ray_origin, ray_direction, REGULAR, RT_DEFAULT_MIN, RT_DEFAULT_MAX);
	picture[ get_global_id(0) * get_global_size(1) + get_global_id(1) ] = make_color( trace( ray ) );
}

__kernel void raytrace( float3 U, float3 V, float3 W, float3 eye, __global uint* picture )
{
	pinhole(U, V, W, eye, picture);
}


#define REGULAR 0
#define RT_DEFAULT_MIN 0.001
#define RT_DEFAULT_MAX 10000.

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

typedef struct {
	float3 normal;
	float t;
	float2 tex;
} Result;

__constant Result MISS =
{
	{ 0.f, 0.f, 0.f },
	-1.f,
	{0.f, 0.f}
};

typedef struct {
	float3 origin;
	float3 direction;
	int type;
	float min;
	float max;
} Ray;

typedef struct {
	float3 center;
	float radius;
} Sphere;

typedef struct {
	float3 v1, v2, v3;
} Triangle;

typedef struct {
	float3 bmin, s1, s2;
} Rectangle;

typedef struct {
	float3 v1, v2, v3, v4;
} Quadrilateral;

typedef struct {
	float3 bmin, bmax;
} AAB;

typedef struct {
	float3 v1, v2, v3;
	float h;
} Box;


static Ray make_ray( float3 o, float3 dir, int type, float min, float max )
{
	Ray ray = {o, dir, type, min, max};
	return ray;
}

inline float4 make_float4(const float a1, const float a2, const float a3, const float a4)
{
	float4 f4 = {a1, a2, a3, a4};
	return f4;
}

inline float3 chip_float4(const float4 a1)
{
	float3 f3 = {a1.x, a1.y, a1.z};
	return f3;
}

inline float4 extend_float3(const float3 a1, const float a2)
{
	float4 f4 = {a1.x, a1.y, a1.z, a2};
	return f4;
}

inline float3 make_float3(const float a1, const float a2, const float a3)
{
	float3 f3 = {a1, a2, a3};
	return f3;
}

inline float2 chip_float3(const float3 a1)
{
	float2 f2 = {a1.x, a1.y};
	return f2;
}



inline float3 fminf3(const float3 a, const float3 b)
{
	return make_float3(fmin(a.x,b.x), fmin(a.y,b.y), fmin(a.z,b.z));
}

inline float fminf(const float3 a)
{
 return fmin(fmin(a.x, a.y), a.z);
}

inline float3 fmaxf3(const float3 a, const float3 b)
{
	return make_float3(fmax(a.x,b.x), fmax(a.y,b.y), fmax(a.z,b.z));
}

inline float fmaxf(const float3 a)
{
 return fmax(fmax(a.x, a.y), a.z);
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


static Result intersect_AAB( Ray ray, float3 bmin, float3 bmax )
{

	// Simple Axis Aligned Box intersection
	float3 t_0    = (bmin - ray.origin)/ray.direction;
	float3 t_1    = (bmax - ray.origin)/ray.direction;
	float t_near = fmaxf( fminf3( t_0, t_1 ) );
	float t_far  = fminf( fmaxf3( t_0, t_1 ) );

	if(t_near >= t_far)
	{
		// Does not intersect
		return MISS;
	}

	// Make sure we pick an intersection that's not behind the origin
	if(t_near < 0.)
	{
		if(t_far < 0.)
		{
			return MISS;
		}
		t_near = t_far;
	}
	
	Result res =
	{
		{ 0.f, 0.f, 0.f },
		t_near,
		{ 0.f, 0.f }
	};
	
	float3 hit = ( ray.origin + t_near*ray.direction );
	float3 dims = bmax - bmin;

	if( t_near == t_0.x )
	{
		res.normal = make_float3( 1.f, 0.f, 0.f );
		res.tex = hit.zy / dims.zy;
		res.tex.y = -res.tex.y;
	}
	else if( t_near == t_1.x )
	{
		res.normal = make_float3( 1.f, 0.f, 0.f );
		res.tex = -hit.zy / dims.zy;
	}
	else if( t_near == t_0.y )
	{
		res.normal = make_float3( 0.f, 1.f, 0.f );
		res.tex = -hit.xz / dims.xz;
		res.tex.y = -res.tex.y;
	}
	else if( t_near == t_1.y )
	{
		res.normal = make_float3( 0.f, 1.f, 0.f );
		res.tex = -hit.xz / dims.xz;
	}
	else if( t_near == t_0.z )
	{
		res.normal = make_float3( 0.f, 0.f, 1.f );
		res.tex = -hit.xy / dims.xy;
	}
	else
	{
		res.normal = make_float3( 0.f, 0.f, 1.f );
		res.tex = hit.xy / dims.xy;
		res.tex.y = -res.tex.y;
	}
	
	return res;
}

static float intersect_plane( Ray ray, float3 v1, float3 v2, float3 v3 )
{
	//Step 1: Determine if we are intersecting the triangle's plane.
	float3 u, v, n;
	float3 w0;
	float  a, b;

	u = v2 - v1;
	v = v3 - v1;
	n = cross( u, v );

	w0 = ray.origin - v1;
	a = -dot( n, w0 );
	b = dot( n, ray.direction );
	
	float t = a / b;
	
	return t;
	
}

static Result intersect_box( Ray ray, Box box )
{
	// Simple Axis Aligned Box intersection
	float n1, n2, n3, f1, f2, f3;
	float3 v1, v2, v3, v4, base_normal, v5, v6, v7, v8;
	v1 = box.v1;
	v2 = box.v2;
	v3 = box.v3;
	v4 = v3 + v2 - v1;
	
	
	base_normal = normalize( cross( v1 - v2, v3 - v1 ) );
	
	v5 = v1 + box.h * base_normal;
	v6 = v2 + box.h * base_normal;
	v7 = v3 + box.h * base_normal;
	v8 = v4 + box.h * base_normal;
	
	n1 = intersect_plane( ray, v1, v2, v3 );
	n2 = intersect_plane( ray, v1, v2, v6 );
	n3 = intersect_plane( ray, v1, v3, v7 );
	f1 = intersect_plane( ray, v6, v7, v8 );
	f2 = intersect_plane( ray, v3, v7, v8 );
	f3 = intersect_plane( ray, v2, v6, v8 );
	
	// We now have our planar points, can now follow the AABB algorithm!
	float3 t_0    = make_float3( n1, n2, n3 );
	float3 t_1    = make_float3( f1, f2, f3 );
	float t_near = fmaxf( fminf3( t_0, t_1 ) );
	float t_far  = fminf( fmaxf3( t_0, t_1 ) );

	if(t_near >= t_far)
	{
		// Does not intersect
		return MISS;
	}

	// Make sure we pick an intersection that's not behind the origin
	if(t_near < 0.)
	{
		if(t_far < 0.)
		{
			return MISS;
		}
		t_near = t_far;
	}

	Result res =
	{
		{ 0.f, 0.f, 0.f },
		t_near,
		( ray.origin + t_near*ray.direction ).xz
	};


	if(t_near == t_0.x || t_near == t_1.x)
	{
		res.normal = normalize( cross( v1 - v2, v1 - v3 ) );
	}
	else if(t_near == t_0.y || t_near == t_1.y)
	{
		res.normal = normalize( cross( v1 - v2, v1 - v6 ) );
	}
	else
	{
		res.normal = normalize( cross( v1 - v3, v1 - v7 ) );
	}
	
	
	return res;
}

/*
// Sphere buils down to a simple quadratic euqation
static Result intersect_sphere( Ray ray, float3 center, float radius )
{
	float a,b,c;
	//Move sphere to origin
	float3 o = (ray.origin - center);

	//Solve quadratic equation a*t� + b*t + c
	a = dot(ray.direction, ray.direction);
	b = dot(2.f * o , ray.direction);
	c = dot(o, o) - radius*radius;

	//t = (-b +- sqrt(b� - 4ac))/2a
	float discriminant = b*b - 4*a*c;
	if(discriminant<0.f) return MISS; //No real roots, miss
	discriminant = sqrt(discriminant);
	float t1 = (-b + discriminant) / (2.f * a);
	float t0 = (-b - discriminant) / (2.f * a);

	float3 normal;
	float t;
	if(t0>0.f)
	{
		float3 hit = ray.origin + ray.direction*t0;
		normal = normalize(center - hit);
		t = t0;
	}
	if(t1>0.f)
	{
		float3 hit = ray.origin + ray.direction*t1;
		normal = normalize(center - hit);
		t = t1;
	}
	return extend_float3( normal, t );
}*/

static Result intersect_sphere2( Ray ray, Sphere s )
{
	float a,b,c;
	//Move sphere to origin
	float3 o = (ray.origin - s.center);

	//Solve quadratic equation a*t� + b*t + c
	a = dot(ray.direction, ray.direction);
	b = dot(2.f * o , ray.direction);
	c = dot(o, o) - s.radius*s.radius;

	//t = (-b +- sqrt(b� - 4ac))/2a
	float discriminant = b*b - 4*a*c;
	if(discriminant<0.f) return MISS; //No real roots, miss
	discriminant = sqrt(discriminant);
	float t1 = (-b + discriminant) / (2.f * a);
	float t0 = (-b - discriminant) / (2.f * a);

	float t;
	t = t0;
	if( t <= RT_DEFAULT_MIN ) t = t1;
	else if( t1 < t && t1 > RT_DEFAULT_MIN ) t = t1;
	if( t > RT_DEFAULT_MIN )
	{
		float3 hit = ray.origin + ray.direction*t0;
		
		Result res =
		{
			normalize( s.center - hit ),
			t,
			hit.xz
		};
		return res;
	}
	return MISS;
}

//Checks if a ray intersects a specific triangle
static Result intersect_triangle( Ray ray, float3 v1, float3 v2, float3 v3 )
{
	//Step 1: Determine if we are intersecting the triangle's plane.
	float3 u, v, n;
	float3 w0, w;
	float  r, a, b;

	u = v2 - v1;
	v = v3 - v1;
	n = cross( u, v );

	w0 = ray.origin - v1;
	a = -dot( n, w0 );
	b = dot( n, ray.direction );

	r = a / b;
	if ( r < 0.0 )
	{
		// We're going away from this triangle
		return MISS;
	}
	float3 hit = ray.origin + r * ray.direction;
	w =  hit - v1;

	// The following models two sides of the triangle and then uses them as two linearly
	// independent vectors. Two unique values s and t will give the intersection point
	// as s*A + t*B, and is only inside the triangle if both s and t are positive and
	// s+t <= 1.
	float uu, uv, vv, wu, wv, D;
	uu = dot(u,u);
	uv = dot(u,v);
	vv = dot(v,v);
	wu = dot(w,u);
	wv = dot(w,v);

	D = uv * uv - uu * vv;

    // Get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.f || s > 1.f)         // I is outside T
	{
        return MISS;
	}
    t = (uv * wu - uu * wv) / D;
    if (t < 0.f || (s + t) > 1.f)  // I is outside T
	{
        return MISS;
	}
	
	n = normalize(n);
	
	Result res = 
	{
		n,
		r,
		hit.xz
	};
	
	return res;
}

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
					nyligst = intersect_sphere2( ray, sphere );
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

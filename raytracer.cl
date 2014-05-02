#define REGULAR 0
#define RT_DEFAULT_MIN 0.001
#define RT_DEFAULT_MAX 10000.
#define MISS make_float4( 0.f, 0.f, 0.f, RT_DEFAULT_MAX +1.f )
static inline unsigned gid( void )
{
	return get_global_id(1) * get_global_size(0) + get_global_id(0);
}

typedef struct{
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
	float3 bmin, bmax, s1;
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
	uint color = 255u;
	color = color << 8u;
	color += (uint)(fabs(in.x)*255.f);
	color = color << 8u;
	color += (uint)(fabs(in.y)*255.f);
	color = color << 8u;
	color |= (uint)(fabs(in.z)*255.f);
	return color;
}


static float4 intersect_AABB( Ray ray, float3 bmin, float3 bmax )
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


	if(t_near == t_0.x || t_near == t_1.x)
	{
		return make_float4( 1.f, 0.f, 0.f, t_near );
	}
	else if(t_near == t_0.y || t_near == t_1.y)
	{
		return make_float4( 0.f, 1.f, 0.f, t_near );
	}
	else
	{
		return make_float4( 0.f, 0.f, 1.f, t_near );
	}
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
	
	return a / b;
	
}

static float4 intersect_box( Ray ray, Box box )
{
	// Simple Axis Aligned Box intersection
	float n1, n2, n3, f1, f2, f3;
	float3 v1, v2, v3, v4, v5, v6, v7, v8;
	v1 = box.bmin;
	v2 = box.bmin + box.s1;
	v8 = box.bmax;
	v7 = box.bmax - box.s1;
	v3 = v7 - make_float3( 0.f, box.h, 0.f );
	//v4 = v8 - make_float3( 0.f, box.h, 0.f );
	//v5 = v1 + make_float3( 0.f, box.h, 0.f );
	v6 = v2 + make_float3( 0.f, box.h, 0.f );
	
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


	if(t_near == t_0.x || t_near == t_1.x)
	{
		return extend_float3( normalize( cross( v1 - v2, v1 - v3 ) ), t_near );
	}
	else if(t_near == t_0.y || t_near == t_1.y)
	{
		return extend_float3( normalize( cross( v1 - v2, v1 - v6 ) ), t_near );
	}
	else
	{
		return extend_float3( normalize( cross( v1- v3, v1 - v7 ) ), t_near );
	}
}

// Sphere buils down to a simple quadratic euqation
static float4 intersect_sphere( Ray ray, float3 center, float radius )
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
}

static float4 intersect_sphere2( Ray ray, Sphere s )
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

	float3 normal;
	float t;
	t = t0;
	if( t <= RT_DEFAULT_MIN ) t = t1;
	else if( t1 < t && t1 > RT_DEFAULT_MIN ) t = t1;
	if( t > RT_DEFAULT_MIN )
	{
		float3 hit = ray.origin + ray.direction*t0;
		normal = normalize( s.center - hit );
		return extend_float3( normal, t );
	}
	return MISS;
}

//Checks if a ray intersects a specific triangle
static float4 intersect_triangle( Ray ray, float3 v1, float3 v2, float3 v3 )
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
	
	return extend_float3( n, r );
}


__kernel void raytrace( float3 U, float3 V, float3 W, float3 eye, __global uint* picture,
	/* Sphere parameters */
	__global float3* sphere_centers, __global float* sphere_radi, int nspheres,
	/* Triangle parameters */
	__global float3* triangles, int ntriangles
	/* Box parameters */
	
	)
{
	// CAMERA
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
	
	
	// RAY TRACING
	
	float4 closest = MISS;
	float4 nyligst;
	
	nyligst = intersect_sphere( ray, make_float3(0.f, 0.f, -1.5f), .5f );
	if( nyligst.w > RT_DEFAULT_MIN && nyligst.w < closest.w ) closest = nyligst;
	
	nyligst = intersect_AABB( ray, make_float3(1.f, 0.f, -0.5f), make_float3(2.f, 1.f, 0.5f) );
	if( nyligst.w > RT_DEFAULT_MIN && nyligst.w < closest.w ) closest = nyligst;
	
	nyligst = intersect_triangle( ray, make_float3(-1.f, 0.f, 0.f), make_float3(-2.f, 1.f, 0.f), make_float3( -1.5f, 1.f, 1.f ) );
	if( nyligst.w > RT_DEFAULT_MIN && nyligst.w < closest.w ) closest = nyligst;
	
	Box testbox = {
		make_float3( 2.f, 2.f, 2.f ),
		make_float3( 2.f, 4.f, 6.f ),
		make_float3( 2.f, 0.f, 2.f ),
		2.f
	};
	
	nyligst = intersect_box( ray, testbox );
	if( nyligst.w > RT_DEFAULT_MIN && nyligst.w < closest.w ) closest = nyligst;

	
	for( int i = 0; i < nspheres; i++ )
	{
		Sphere sphere = { sphere_centers[i], sphere_radi[i] };
		nyligst = intersect_sphere2(ray, sphere);
		if( nyligst.w > RT_DEFAULT_MIN && nyligst.w < closest.w ) closest = nyligst;
	}
	
	for( int i = 0; i < ntriangles; i++ )
	{
		nyligst = intersect_triangle(ray, triangles[i*3], triangles[i*3+1], triangles[i*3+2] );
		if( nyligst.w > RT_DEFAULT_MIN && nyligst.w < closest.w ) closest = nyligst;
	}
	
	float3 result = make_float3( closest.x, closest.y, closest.z );
	
	picture[ gid( ) ] = make_color( result );
}
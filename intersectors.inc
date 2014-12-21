
static float2 f3xy( float3 arg )
{
	return make_float2( arg.x, arg.y );
}
static float2 f3xz( float3 arg )
{
	return make_float2( arg.x, arg.z );
}
static float2 f3yz( float3 arg )
{
	return make_float2( arg.y, arg.z );
}
static float2 f3yx( float3 arg )
{
	return make_float2( arg.y, arg.x );
}
static float2 f3zx( float3 arg )
{
	return make_float2( arg.y, arg.z );
}
static float2 f3zy( float3 arg )
{
	return make_float2( arg.z, arg.y );
}

static float bound( Ray ray, float3 bmin, float3 bmax )
{
	
	// Simple Axis Aligned Box intersection
	float3 t_0    = (bmin - ray.origin)/ray.direction;
	float3 t_1    = (bmax - ray.origin)/ray.direction;
	float t_near = fmaxf( fminf3( t_0, t_1 ) );
	float t_far  = fminf( fmaxf3( t_0, t_1 ) );
	if(t_near >= t_far)
	{
		// Does not intersect
		return -1.f;
	}

	// Make sure we pick an intersection that's not behind the origin
	if(t_near < 0.)
	{
		if(t_far < 0.)
		{
			return -1.f;
		}
		return t_far;
	}
	
	return t_near;
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
		res.tex = f3zy(hit) / f3zy(dims);
		res.tex.y = -res.tex.y;
	}
	else if( t_near == t_1.x )
	{
		res.normal = make_float3( 1.f, 0.f, 0.f );
		res.tex = -f3zy(hit) / f3zy(dims);
	}
	else if( t_near == t_0.y )
	{
		res.normal = make_float3( 0.f, 1.f, 0.f );
		res.tex = -f3xz(hit) / f3xz(dims);
		res.tex.y = -res.tex.y;
	}
	else if( t_near == t_1.y )
	{
		res.normal = make_float3( 0.f, 1.f, 0.f );
		res.tex = -f3xz(hit) / f3xz(dims);
	}
	else if( t_near == t_0.z )
	{
		res.normal = make_float3( 0.f, 0.f, 1.f );
		res.tex = -f3xy(hit) / f3xy(dims);
	}
	else
	{
		res.normal = make_float3( 0.f, 0.f, 1.f );
		res.tex = f3xy(hit) / f3xy(dims);
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
	v1 = box.s1;
	v2 = box.s2;
	v3 = box.s3;
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
		f3xz(( ray.origin + t_near*ray.direction ))
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

static Result intersect_sphere( Ray ray, Sphere s )
{
	float a,b,c;
	//Move sphere to origin
	float3 o = (ray.origin - s.center);

	//Solve quadratic equation a*t² + b*t + c
	a = dot(ray.direction, ray.direction);
	b = dot(2.f * o , ray.direction);
	c = dot(o, o) - s.radius*s.radius;

	//t = (-b +- sqrt(b² - 4ac))/2a
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
			f3xz(hit)
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
		f3xz(hit)
	};
	
	return res;
}
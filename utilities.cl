

#define printg3(a) if( gid() == 0 )printf( #a ": %g, %g, %g\n", a.x, a.y, a.z );
#define printg(a) if( gid() == 0 )printf( #a ": %g\n", a );
#define printi(a) if( gid() == 0 )printf( #a ": %d\n", a );
#define printu(a) if( gid() == 0 )printf( #a ": %u\n", a );
#define printg3all(a) printf( #a ": %g, %g, %g\n", a.x, a.y, a.z );
#define printgall(a) printf( #a ": %g\n", a );
#define printiall(a) printf( #a ": %d\n", a );
#define printuall(a) printf( #a ": %u\n", a );

#ifdef PRINT
#define ifprint(...) if( get_global_id(0) == 160 && get_global_id(1) == 120 ){ printf(__VA_ARGS__);}
#else
#define ifprint(...) 
#endif

enum PrimitiveType { Sphere_t, Triangle_t, Rectangle_t, Quadrilateral_t, AAB_t, Box_t };



static inline unsigned gid( void )
{
	return get_global_id(1) * get_global_size(0) + get_global_id(0);
}

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

static float gmod( float a1, float a2 )
{
	float ratio = a1 / a2;
	a1 = (ratio - floor(ratio))*a2;
	if(a2 - a1 < 5e-5) return 0.f;
	return a1;
}
#ifndef vtypes_h
#define vtypes_h
#include <math.h>
#include <CL/cl.h>
#include <CL/cl_platform.h>
/* float2 functions */
/******************************************************************************/

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Use opencl types to guarantee being spatially identical.
typedef cl_float2 float2;
typedef cl_float3 float3;
typedef cl_float4 float4;
typedef cl_int2 int2;
typedef cl_int3 int3;
typedef cl_int4 int4;
typedef cl_uint2 uint2;
typedef cl_uint3 uint3;
typedef cl_uint4 uint4;


inline unsigned int min( unsigned int a, unsigned int b )
{
	return a < b ? a : b;
}
inline unsigned int max( unsigned int a, unsigned int b )
{
	return a > b ? a : b;
}

inline int min( int a, int b )
{
	return a < b ? a : b;
}
inline int max( int a, int b )
{
	return a > b ? a : b;
}

/* constructors */
inline float2 make_float2(const float a1, const float a2)
{
	float2 f2 = {a1, a2};
	return f2;
}

inline float2 make_float2(const float s)
{
 return make_float2(s, s);
}
inline float2 make_float2(const int2& a)
{
 return make_float2(float(a.x), float(a.y));
}
inline float2 make_float2(const uint2& a)
{
 return make_float2(float(a.x), float(a.y));
}

/* negate */
inline float2 operator-(const float2& a)
{
 return make_float2(-a.x, -a.y);
}

/* min */
inline float2 fminf(const float2& a, const float2& b)
{
	return make_float2(fminf(a.x,b.x), fminf(a.y,b.y));
}
inline float2 fminf(const float2& a, const float2& b, const float2& c)
{
	return fminf( fminf( a, b ), c );
}
inline float fminf(const float2& a)
{
 return fminf(a.x, a.y);
}

/* max */
inline float2 fmaxf(const float2& a, const float2& b)
{
	return make_float2(fmaxf(a.x,b.x), fmaxf(a.y,b.y));
}
inline float2 fmaxf(const float2& a, const float2& b, const float2& c)
{
	return fmaxf( fmaxf( a, b ), c );
}
inline float fmaxf(const float2& a)
{
 return fmaxf(a.x, a.y);
}

/* add */
inline float2 operator+(const float2& a, const float2& b)
{
 return make_float2(a.x + b.x, a.y + b.y);
}
inline float2 operator+(const float2& a, const float b)
{
 return make_float2(a.x + b, a.y + b);
}
inline float2 operator+(const float a, const float2& b)
{
 return make_float2(a + b.x, a + b.y);
}
inline void operator+=(float2& a, const float2& b)
{
 a.x += b.x; a.y += b.y;
}

/* subtract */
inline float2 operator-(const float2& a, const float2& b)
{
 return make_float2(a.x - b.x, a.y - b.y);
}
inline float2 operator-(const float2& a, const float b)
{
 return make_float2(a.x - b, a.y - b);
}
inline float2 operator-(const float a, const float2& b)
{
 return make_float2(a - b.x, a - b.y);
}
inline void operator-=(float2& a, const float2& b)
{
 a.x -= b.x; a.y -= b.y;
}

/* multiply */
inline float2 operator*(const float2& a, const float2& b)
{
 return make_float2(a.x * b.x, a.y * b.y);
}
inline float2 operator*(const float2& a, const float s)
{
 return make_float2(a.x * s, a.y * s);
}
inline float2 operator*(const float s, const float2& a)
{
 return make_float2(a.x * s, a.y * s);
}
inline void operator*=(float2& a, const float2& s)
{
 a.x *= s.x; a.y *= s.y;
}
inline void operator*=(float2& a, const float s)
{
 a.x *= s; a.y *= s;
}

/* divide */
inline float2 operator/(const float2& a, const float2& b)
{
 return make_float2(a.x / b.x, a.y / b.y);
}
inline float2 operator/(const float2& a, const float s)
{
 float inv = 1.0f / s;
 return a * inv;
}
inline float2 operator/(const float s, const float2& a)
{
 return make_float2( s/a.x, s/a.y );
}
inline void operator/=(float2& a, const float s)
{
 float inv = 1.0f / s;
 a *= inv;
}

/* lerp */
inline float2 lerp(const float2& a, const float2& b, const float t)
{
 return a + t*(b-a);
}

/* bilerp */
inline float2 bilerp(const float2& x00, const float2& x10, const float2& x01, const float2& x11,
                     const float u, const float v)
{
 return lerp( lerp( x00, x10, u ), lerp( x01, x11, u ), v );
}

/* dot product */
inline float dot(const float2& a, const float2& b)
{
 return a.x * b.x + a.y * b.y;
}

/* length */
inline float length(const float2& v)
{
 return sqrtf(dot(v, v));
}

/* normalize */
inline float2 normalize(const float2& v)
{
 float invLen = 1.0f / sqrtf(dot(v, v));
 return v * invLen;
}

/* floor */
inline float2 floor(const float2& v)
{
 return make_float2(::floorf(v.x), ::floorf(v.y));
}

/* reflect */
inline float2 reflect(const float2& i, const float2& n)
{
	return i - 2.0f * n * dot(n,i);
}

/* faceforward */
/* Returns N if dot(i, nref) > 0; else -N; */
/* Typical usage is N = faceforward(N, -ray.dir, N); */
/* Note that this is opposite of what faceforward does in Cg and GLSL */
inline float2 faceforward(const float2& n, const float2& i, const float2& nref)
{
 return n * copysignf( 1.0f, dot(i, nref) );
}

/* exp */
inline float2 expf(const float2& v)
{
 return make_float2(::expf(v.x), ::expf(v.y));
}





/* float4 functions */
/******************************************************************************/

/* constructors */
inline float4 make_float4(const float a1, const float a2, const float a3, const float a4)
{
	float4 f4 = {a1, a2, a3, a4};
	return f4;
}
inline float4 make_float4(const float s)
{
 return make_float4(s, s, s, s);
}
inline float4 make_float4(const float3& a)
{
 return make_float4(a.x, a.y, a.z, 0.0f);
}
inline float4 make_float4(const int4& a)
{
 return make_float4(float(a.x), float(a.y), float(a.z), float(a.w));
}
inline float4 make_float4(const uint4& a)
{
 return make_float4(float(a.x), float(a.y), float(a.z), float(a.w));
}

/* negate */
inline float4 operator-(const float4& a)
{
 return make_float4(-a.x, -a.y, -a.z, -a.w);
}

/* min */
inline float4 fminf(const float4& a, const float4& b)
{
	return make_float4(fminf(a.x,b.x), fminf(a.y,b.y), fminf(a.z,b.z), fminf(a.w,b.w));
}
inline float4 fminf(const float4& a, const float4& b, const float4& c)
{
	return fminf( fminf( a, b ), c );
}
inline float fminf(const float4& a)
{
 return fminf(fminf(a.x, a.y), fminf(a.z, a.w));
}

/* max */
inline float4 fmaxf(const float4& a, const float4& b)
{
	return make_float4(fmaxf(a.x,b.x), fmaxf(a.y,b.y), fmaxf(a.z,b.z), fmaxf(a.w,b.w));
}
inline float4 fmaxf(const float4& a, const float4& b, const float4& c)
{
	return fmaxf( fmaxf( a, b ), c );
}
inline float fmaxf(const float4& a)
{
 return fmaxf(fmaxf(a.x, a.y), fmaxf(a.z, a.w));
}

/* add */
inline float4 operator+(const float4& a, const float4& b)
{
 return make_float4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}
inline float4 operator+(const float4& a, const float b)
{
 return make_float4(a.x + b, a.y + b, a.z + b, a.w + b);
}
inline float4 operator+(const float a, const float4& b)
{
 return make_float4(a + b.x, a + b.y, a + b.z, a + b.w);
}
inline void operator+=(float4& a, const float4& b)
{
 a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
}

/* subtract */
inline float4 operator-(const float4& a, const float4& b)
{
 return make_float4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}
inline float4 operator-(const float4& a, const float b)
{
 return make_float4(a.x - b, a.y - b, a.z - b, a.w - b);
}
inline float4 operator-(const float a, const float4& b)
{
 return make_float4(a - b.x, a - b.y, a - b.z, a - b.w);
}
inline void operator-=(float4& a, const float4& b)
{
 a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
}

/* multiply */
inline float4 operator*(const float4& a, const float4& s)
{
 return make_float4(a.x * s.x, a.y * s.y, a.z * s.z, a.w * s.w);
}
inline float4 operator*(const float4& a, const float s)
{
 return make_float4(a.x * s, a.y * s, a.z * s, a.w * s);
}
inline float4 operator*(const float s, const float4& a)
{
 return make_float4(a.x * s, a.y * s, a.z * s, a.w * s);
}
inline void operator*=(float4& a, const float4& s)
{
 a.x *= s.x; a.y *= s.y; a.z *= s.z; a.w *= s.w;
}
inline void operator*=(float4& a, const float s)
{
 a.x *= s; a.y *= s; a.z *= s; a.w *= s;
}

/* divide */
inline float4 operator/(const float4& a, const float4& b)
{
 return make_float4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}
inline float4 operator/(const float4& a, const float s)
{
 float inv = 1.0f / s;
 return a * inv;
}
inline float4 operator/(const float s, const float4& a)
{
 return make_float4( s/a.x, s/a.y, s/a.z, s/a.w );
}
inline void operator/=(float4& a, const float s)
{
 float inv = 1.0f / s;
 a *= inv;
}

/* dot product */
inline float dot(const float4& a, const float4& b)
{
 return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

/* length */
inline float length(const float4& r)
{
 return sqrtf(dot(r, r));
}

/* normalize */
inline float4 normalize(const float4& v)
{
 float invLen = 1.0f / sqrtf(dot(v, v));
 return v * invLen;
}

/* floor */
inline float4 floor(const float4& v)
{
 return make_float4(::floorf(v.x), ::floorf(v.y), ::floorf(v.z), ::floorf(v.w));
}

/* reflect */
inline float4 reflect(const float4& i, const float4& n)
{
	return i - 2.0f * n * dot(n,i);
}

/* faceforward */
/* Returns N if dot(i, nref) > 0; else -N; */
/* Typical usage is N = faceforward(N, -ray.dir, N); */
/* Note that this is opposite of what faceforward does in Cg and GLSL */
inline float4 faceforward(const float4& n, const float4& i, const float4& nref)
{
 return n * copysignf( 1.0f, dot(i, nref) );
}

/* exp */
inline float4 expf(const float4& v)
{
 return make_float4(::expf(v.x), ::expf(v.y), ::expf(v.z), ::expf(v.w));
}

/* float3 functions */
/******************************************************************************/

/* constructors */
inline float3 make_float3(const float a1, const float a2, const float a3)
{
	float3 f3 = {a1, a2, a3};
	return f3;
}

inline float3 make_float3(const float s)
{
 return make_float3(s, s, s);
}
inline float3 make_float3(const float2& a)
{
 return make_float3(a.x, a.y, 0.0f);
}
inline float3 make_float3(const int3& a)
{
 return make_float3(float(a.x), float(a.y), float(a.z));
}
inline float3 make_float3(const uint3& a)
{
 return make_float3(float(a.x), float(a.y), float(a.z));
}


/* min */
inline float3 fminf3(const float3& a, const float3& b)
{
	return make_float3(fminf(a.x,b.x), fminf(a.y,b.y), fminf(a.z,b.z));
}
inline float fminf3(const float3& a)
{
 return fminf(fminf(a.x, a.y), a.z);
}

/* max */
inline float3 fmaxf3(const float3& a, const float3& b)
{
	return make_float3(fmaxf(a.x,b.x), fmaxf(a.y,b.y), fmaxf(a.z,b.z));
}
inline float fmaxf3(const float3& a)
{
 return fmaxf(fmaxf(a.x, a.y), a.z);
}


/* dot product */
inline float dot3(const float3& a, const float3& b)
{
 return a.x * b.x + a.y * b.y + a.z * b.z;
}

/* cross product */
inline float3 cross(const float3& a, const float3& b)
{
 return make_float3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

/* length */
inline float length3(const float3& v)
{
 return sqrtf(dot(v, v));
}

/* normalize */
inline float3 normalize3(const float3& v)
{
 float invLen = 1.0f / sqrtf(dot(v, v));
 return v * invLen;
}

/* reflect */
inline float3 reflect3(const float3& i, const float3& n)
{
	return i - 2.0f * n * dot(n,i);
}

/* faceforward */
/* Returns N if dot(i, nref) > 0; else -N; */
/* Typical usage is N = faceforward(N, -ray.dir, N); */
/* Note that this is opposite of what faceforward does in Cg and GLSL */
inline float3 faceforward3(const float3& n, const float3& i, const float3& nref)
{
 return n * copysignf( 1.0f, dot(i, nref) );
}


/* int2 functions */
/******************************************************************************/

/* additional constructors */
inline int2 make_int2(const int a1, const int a2)
{
	int2 i2 = {a1, a2};
	return i2;
}
inline int2 make_int2(const int s)
{
 return make_int2(s, s);
}
inline int2 make_int2(const float2& a)
{
 return make_int2(int(a.x), int(a.y));
}

/* negate */
inline int2 operator-(const int2& a)
{
 return make_int2(-a.x, -a.y);
}

/* min */
inline int2 min(const int2& a, const int2& b)
{
 return make_int2(min(a.x,b.x), min(a.y,b.y));
}

/* max */
inline int2 max(const int2& a, const int2& b)
{
 return make_int2(max(a.x,b.x), max(a.y,b.y));
}

/* add */
inline int2 operator+(const int2& a, const int2& b)
{
 return make_int2(a.x + b.x, a.y + b.y);
}
inline void operator+=(int2& a, const int2& b)
{
 a.x += b.x; a.y += b.y;
}

/* subtract */
inline int2 operator-(const int2& a, const int2& b)
{
 return make_int2(a.x - b.x, a.y - b.y);
}
inline int2 operator-(const int2& a, const int b)
{
 return make_int2(a.x - b, a.y - b);
}
inline void operator-=(int2& a, const int2& b)
{
 a.x -= b.x; a.y -= b.y;
}

/* multiply */
inline int2 operator*(const int2& a, const int2& b)
{
 return make_int2(a.x * b.x, a.y * b.y);
}
inline int2 operator*(const int2& a, const int s)
{
 return make_int2(a.x * s, a.y * s);
}
inline int2 operator*(const int s, const int2& a)
{
 return make_int2(a.x * s, a.y * s);
}
inline void operator*=(int2& a, const int s)
{
 a.x *= s; a.y *= s;
}

/* equality */
inline bool operator==(const int2& a, const int2& b)
{
 return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const int2& a, const int2& b)
{
 return a.x != b.x || a.y != b.y;
}


/* int3 functions */
/******************************************************************************/

/* constructors */
inline int3 make_int3(const int a1, const int a2, const int a3)
{
	int3 i3 = {a1, a2, a3};
	return i3;
}
inline int3 make_int3(const int s)
{
 return make_int3(s, s, s);
}
inline int3 make_int3(const float3& a)
{
 return make_int3(int(a.x), int(a.y), int(a.z));
}


/* int4 functions */
/******************************************************************************/

/* constructors */
inline int4 make_int4(const int a1, const int a2, const int a3, const int a4)
{
	int4 i4 = {a1, a2, a3, a4};
	return i4;
}
inline int4 make_int4(const int s)
{
 return make_int4(s, s, s, s);
}
inline int4 make_int4(const float4& a)
{
 return make_int4((int)a.x, (int)a.y, (int)a.z, (int)a.w);
}

/* negate */
inline int4 operator-(const int4& a)
{
 return make_int4(-a.x, -a.y, -a.z, -a.w);
}

/* min */
inline int4 min(const int4& a, const int4& b)
{
 return make_int4(min(a.x,b.x), min(a.y,b.y), min(a.z,b.z), min(a.w,b.w));
}

/* max */
inline int4 max(const int4& a, const int4& b)
{
 return make_int4(max(a.x,b.x), max(a.y,b.y), max(a.z,b.z), max(a.w,b.w));
}

/* add */
inline int4 operator+(const int4& a, const int4& b)
{
 return make_int4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}
inline void operator+=(int4& a, const int4& b)
{
 a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
}

/* subtract */
inline int4 operator-(const int4& a, const int4& b)
{
 return make_int4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

inline void operator-=(int4& a, const int4& b)
{
 a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
}

/* multiply */
inline int4 operator*(const int4& a, const int4& b)
{
 return make_int4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}
inline int4 operator*(const int4& a, const int s)
{
 return make_int4(a.x * s, a.y * s, a.z * s, a.w * s);
}
inline int4 operator*(const int s, const int4& a)
{
 return make_int4(a.x * s, a.y * s, a.z * s, a.w * s);
}
inline void operator*=(int4& a, const int s)
{
 a.x *= s; a.y *= s; a.z *= s; a.w *= s;
}

/* divide */
inline int4 operator/(const int4& a, const int4& b)
{
 return make_int4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}
inline int4 operator/(const int4& a, const int s)
{
 return make_int4(a.x / s, a.y / s, a.z / s, a.w / s);
}
inline int4 operator/(const int s, const int4& a)
{
 return make_int4(s / a.x, s / a.y, s / a.z, s / a.w);
}
inline void operator/=(int4& a, const int s)
{
 a.x /= s; a.y /= s; a.z /= s; a.w /= s;
}

/* equality */
inline bool operator==(const int4& a, const int4& b)
{
 return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

inline bool operator!=(const int4& a, const int4& b)
{
 return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}



/* uint2 functions */
/******************************************************************************/

/* constructors */
inline uint2 make_uint2(const unsigned int a1, const unsigned int a2)
{
	uint2 ui2 = {a1, a2};
	return ui2;
}
inline uint2 make_uint2(const unsigned int s)
{
 return make_uint2(s, s);
}
inline uint2 make_uint2(const float2& a)
{
 return make_uint2((unsigned int)a.x, (unsigned int)a.y);
}

/* min */
inline uint2 min(const uint2& a, const uint2& b)
{
 return make_uint2(min(a.x,b.x), min(a.y,b.y));
}

/* max */
inline uint2 max(const uint2& a, const uint2& b)
{
 return make_uint2(max(a.x,b.x), max(a.y,b.y));
}

/* add */
inline uint2 operator+(const uint2& a, const uint2& b)
{
 return make_uint2(a.x + b.x, a.y + b.y);
}
inline void operator+=(uint2& a, const uint2& b)
{
 a.x += b.x; a.y += b.y;
}

/* subtract */
inline uint2 operator-(const uint2& a, const uint2& b)
{
 return make_uint2(a.x - b.x, a.y - b.y);
}
inline uint2 operator-(const uint2& a, const unsigned int b)
{
 return make_uint2(a.x - b, a.y - b);
}
inline void operator-=(uint2& a, const uint2& b)
{
 a.x -= b.x; a.y -= b.y;
}

/* multiply */
inline uint2 operator*(const uint2& a, const uint2& b)
{
 return make_uint2(a.x * b.x, a.y * b.y);
}
inline uint2 operator*(const uint2& a, const unsigned int s)
{
 return make_uint2(a.x * s, a.y * s);
}
inline uint2 operator*(const unsigned int s, const uint2& a)
{
 return make_uint2(a.x * s, a.y * s);
}
inline void operator*=(uint2& a, const unsigned int s)
{
 a.x *= s; a.y *= s;
}

/* equality */
inline bool operator==(const uint2& a, const uint2& b)
{
 return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const uint2& a, const uint2& b)
{
 return a.x != b.x || a.y != b.y;
}


/* uint3 functions */
/******************************************************************************/

/* constructors */
inline uint3 make_uint3(const unsigned int a1, const unsigned int a2, const unsigned int a3)
{
	uint3 ui3 = {a1, a2, a3};
	return ui3;
}
inline uint3 make_uint3(const unsigned int s)
{
 return make_uint3(s, s, s);
}
inline uint3 make_uint3(const float3& a)
{
 return make_uint3((unsigned int)a.x, (unsigned int)a.y, (unsigned int)a.z);
}


/* uint4 functions */
/******************************************************************************/

/* constructors */
inline uint4 make_uint4(const unsigned int a1, const unsigned int a2, const unsigned int a3, const unsigned int a4)
{
	uint4 ui4 = {a1, a2, a3, a4};
	return ui4;
}
inline uint4 make_uint4(const unsigned int s)
{
 return make_uint4(s, s, s, s);
}
inline uint4 make_uint4(const float4& a)
{
 return make_uint4((unsigned int)a.x, (unsigned int)a.y, (unsigned int)a.z, (unsigned int)a.w);
}

/* min */
inline uint4 min(const uint4& a, const uint4& b)
{
 return make_uint4(min(a.x,b.x), min(a.y,b.y), min(a.z,b.z), min(a.w,b.w));
}

/* max */
inline uint4 max(const uint4& a, const uint4& b)
{
 return make_uint4(max(a.x,b.x), max(a.y,b.y), max(a.z,b.z), max(a.w,b.w));
}

/* add */
inline uint4 operator+(const uint4& a, const uint4& b)
{
 return make_uint4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}
inline void operator+=(uint4& a, const uint4& b)
{
 a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
}

/* subtract */
inline uint4 operator-(const uint4& a, const uint4& b)
{
 return make_uint4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

inline void operator-=(uint4& a, const uint4& b)
{
 a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
}

/* multiply */
inline uint4 operator*(const uint4& a, const uint4& b)
{
 return make_uint4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}
inline uint4 operator*(const uint4& a, const unsigned int s)
{
 return make_uint4(a.x * s, a.y * s, a.z * s, a.w * s);
}
inline uint4 operator*(const unsigned int s, const uint4& a)
{
 return make_uint4(a.x * s, a.y * s, a.z * s, a.w * s);
}
inline void operator*=(uint4& a, const unsigned int s)
{
 a.x *= s; a.y *= s; a.z *= s; a.w *= s;
}

/* divide */
inline uint4 operator/(const uint4& a, const uint4& b)
{
 return make_uint4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}
inline uint4 operator/(const uint4& a, const unsigned int s)
{
 return make_uint4(a.x / s, a.y / s, a.z / s, a.w / s);
}
inline uint4 operator/(const unsigned int s, const uint4& a)
{
 return make_uint4(s / a.x, s / a.y, s / a.z, s / a.w);
}
inline void operator/=(uint4& a, const unsigned int s)
{
 a.x /= s; a.y /= s; a.z /= s; a.w /= s;
}
/* equality */
inline bool operator==(const uint4& a, const uint4& b)
{
 return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

inline bool operator!=(const uint4& a, const uint4& b)
{
 return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}


/******************************************************************************/

/* Narrowing */


inline int2 make_int2(const int4& v0) { return make_int2( v0.x, v0.y ); }
inline int3 make_int3(const int4& v0) { return make_int3( v0.x, v0.y, v0.z ); }
inline uint2 make_uint2(const uint4& v0) { return make_uint2( v0.x, v0.y ); }
inline uint3 make_uint3(const uint4& v0) { return make_uint3( v0.x, v0.y, v0.z ); }
inline float2 make_float2(const float4& v0) { return make_float2( v0.x, v0.y ); }
inline float3 make_float3(const float4& v0) { return make_float3( v0.x, v0.y, v0.z ); }

/* Assemble from smaller vectors */

inline int3 make_int3(const int v0, const int2& v1) { return make_int3( v0, v1.x, v1.y ); }
inline int3 make_int3(const int2& v0, const int v1) { return make_int3( v0.x, v0.y, v1 ); }
inline int4 make_int4(const int v0, const int v1, const int2& v2) { return make_int4( v0, v1, v2.x, v2.y ); }
inline int4 make_int4(const int v0, const int2& v1, const int v2) { return make_int4( v0, v1.x, v1.y, v2 ); }
inline int4 make_int4(const int2& v0, const int v1, const int v2) { return make_int4( v0.x, v0.y, v1, v2 ); }
inline int4 make_int4(const int v0, const int3& v1) { return make_int4( v0, v1.x, v1.y, v1.z ); }
inline int4 make_int4(const int3& v0, const int v1) { return make_int4( v0.x, v0.y, v0.z, v1 ); }
inline int4 make_int4(const int2& v0, const int2& v1) { return make_int4( v0.x, v0.y, v1.x, v1.y ); }
inline uint3 make_uint3(const unsigned int v0, const uint2& v1) { return make_uint3( v0, v1.x, v1.y ); }
inline uint3 make_uint3(const uint2& v0, const unsigned int v1) { return make_uint3( v0.x, v0.y, v1 ); }
inline uint4 make_uint4(const unsigned int v0, const unsigned int v1, const uint2& v2) { return make_uint4( v0, v1, v2.x, v2.y ); }
inline uint4 make_uint4(const unsigned int v0, const uint2& v1, const unsigned int v2) { return make_uint4( v0, v1.x, v1.y, v2 ); }
inline uint4 make_uint4(const uint2& v0, const unsigned int v1, const unsigned int v2) { return make_uint4( v0.x, v0.y, v1, v2 ); }
inline uint4 make_uint4(const unsigned int v0, const uint3& v1) { return make_uint4( v0, v1.x, v1.y, v1.z ); }
inline uint4 make_uint4(const uint3& v0, const unsigned int v1) { return make_uint4( v0.x, v0.y, v0.z, v1 ); }
inline uint4 make_uint4(const uint2& v0, const uint2& v1) { return make_uint4( v0.x, v0.y, v1.x, v1.y ); }
inline float3 make_float3(const float2& v0, const float v1) { return make_float3(v0.x, v0.y, v1); }
inline float3 make_float3(const float v0, const float2& v1) { return make_float3( v0, v1.x, v1.y ); }
inline float4 make_float4(const float v0, const float v1, const float2& v2) { return make_float4( v0, v1, v2.x, v2.y ); }
inline float4 make_float4(const float v0, const float2& v1, const float v2) { return make_float4( v0, v1.x, v1.y, v2 ); }
inline float4 make_float4(const float2& v0, const float v1, const float v2) { return make_float4( v0.x, v0.y, v1, v2 ); }
inline float4 make_float4(const float v0, const float3& v1) { return make_float4( v0, v1.x, v1.y, v1.z ); }
inline float4 make_float4(const float3& v0, const float v1) { return make_float4( v0.x, v0.y, v0.z, v1 ); }
inline float4 make_float4(const float2& v0, const float2& v1) { return make_float4( v0.x, v0.y, v1.x, v1.y ); }

#endif

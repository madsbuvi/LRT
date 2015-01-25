
#ifndef shared_h
#define shared_h

#ifndef DEVICE_SIDE

#	include "vtypes.h"
#	define __constant const

#else

static inline float4 make_float4(const float a1, const float a2, const float a3, const float a4)
{
	float4 f4 = {a1, a2, a3, a4};
	return f4;
}

static inline float3 chip_float4(const float4 a1)
{
	float3 f3 = {a1.x, a1.y, a1.z};
	return f3;
}

static inline float4 extend_float3(const float3 a1, const float a2)
{
	float4 f4 = {a1.x, a1.y, a1.z, a2};
	return f4;
}

static inline float3 make_float3(const float a1, const float a2, const float a3)
{
	float3 f3 = {a1, a2, a3};
	return f3;
}

static inline float2 chip_float3(const float3 a1)
{
	float2 f2 = {a1.x, a1.y};
	return f2;
}

static inline float2 make_float2(const float a1, const float a2)
{
	float2 f2 = {a1, a2};
	return f2;
}

#endif


#define RT_DEFAULT_MIN 0.001
#define RT_DEFAULT_MAX 10000.


typedef struct {
	float3 origin;
	float3 direction;
	int type;
	float min;
	float max;
	
#ifdef DEVICE_SIDE
	float relevance;
#endif
} Ray;

typedef struct {
	float3 normal;
	float t;
	float2 tex;
	unsigned geo;
	float3 result;
	bool trace;
	bool recur;
	bool ready;
	Ray reflect;
} Result;

typedef struct {
	float3 center;
	float radius;
} Sphere;

typedef struct {
	float3 v1, v2, v3;
} Triangle;

typedef struct {
	float3 v1, v2, v3, v4;
} Quadrilateral;

typedef struct {
	float3 bmin, bmax;
} AAB_s;

typedef struct {
	float3 s1, s2, s3;
	float h;
} Box;


__constant Result MISS =
{
	{ 0.f, 0.f, 0.f },
	-1.f,
	{0.f, 0.f},
	0,
	{ 0.f, 0.f, 0.f },
	0,
	0,
	1
};


static Ray make_ray( float3 o, float3 dir, int type, float min, float max
#ifdef DEVICE_SIDE
	, float relevance
#endif
)
{
	Ray ray = {o, dir, type, min, max
	
#ifdef DEVICE_SIDE
	, relevance
#endif
	};
	return ray;
}
#endif
#ifndef geometry_h
#define geometry_h

#include <vector>
#include "vtypes.h"

enum PrimitiveType { Sphere_t, Triangle_t, Square_t, Rectangle_t, Quadrilateral_t, AAB_t, Box_t, Pyramid_t };

class Primitive
{
	PrimitiveType type;
	public:
	PrimitiveType getType( void ) { return type; };
};

class Geometry
{
	std::vector<Primitive> primitives;
	public:
	int addGeometry( Primitive p ){ primitives.push_back( p ); return primitives.size(); };
};

class Sphere: public Primitive
{
	float radius;
	float3 center;
	public:
	Sphere ( float r, float3 c ){ radius = r; center = c; };
};

#endif
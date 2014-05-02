#include "geometry.h"
Pyramid::Pyramid( float3 v1, float3 v2, float3 v3, float3 v4, float3 apex ){
	Quadrilateral* base = new Quadrilateral( v1, v2, v3, v4 );
	Triangle* s1 = new Triangle( v1, v2, apex );
	Triangle* s2 = new Triangle( v2, v3, apex );
	Triangle* s3 = new Triangle( v3, v4, apex );
	Triangle* s4 = new Triangle( v4, v1, apex );
	addPrimitive( base );
	addPrimitive( s1 );
	addPrimitive( s2 );
	addPrimitive( s3 );
	addPrimitive( s4 );
}

Geometry make_sphere( float3 c, float r )
{
	Sphere* p = new Sphere( c, r );
	Geometry g( p );
	return g;
}

Geometry make_triangle( float3 v1, float3 v2, float3 v3 )
{
	Triangle* p = new Triangle( v1, v2, v3 );
	Geometry g( p );
	return g;
}

Geometry make_rectangle( float3 bmin, float3 s1, float3 s2 )
{
	Rectangle* p = new Rectangle( bmin, s1, s2 );
	Geometry g( p );
	return g;
}

Geometry make_quadrilateral( float3 v1, float3 v2, float3 v3, float3 v4 )
{
	Quadrilateral* p = new Quadrilateral( v1, v2, v3, v4 );
	Geometry g( p );
	return g;
}

Geometry make_AAB( float3 bmin, float3 bmax )
{
	AAB* p = new AAB( bmin, bmax );
	Geometry g( p );
	return g;
}

Geometry make_box( float3 bmin, float3 bmax, float3 s1, float h )
{
	Box* p = new Box( bmin, bmax, s1, h );
	Geometry g( p );
	return g;
}

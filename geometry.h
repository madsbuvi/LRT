#ifndef geometry_h
#define geometry_h

#include <vector>
#include <stdio.h>
#include "vtypes.h"
#include "shaders.h"

enum PrimitiveType { Sphere_t, Triangle_t, Rectangle_t, Quadrilateral_t, AAB_t, Box_t };
enum GeometryType { GEO_GENERIC=0, GEO_AAB=1, GEO_AAP=2  };

typedef struct {
	float3 center;
	float radius;
} Sphere_struct;

typedef struct {
	float3 v1, v2, v3;
} Triangle_struct;

typedef struct {
	float3 bmin, s1, s2;
} Rectangle_struct;

typedef struct {
	float3 v1, v2, v3, v4;
} Quadrilateral_struct;

typedef struct {
	float3 bmin, bmax;
} AAB_struct;

typedef struct {
	float3 s1, s2, s3;
	float h;
} Box_struct;

class Primitive
{
	protected:
	PrimitiveType type;
	public:
	PrimitiveType getType( void ) { return type; };
};

class Geometry
{
	Shader* shader;
	protected:
	std::vector<Primitive*> primitives;
	public:
	Geometry( void ){ shader = NULL; };
	int addPrimitive( Primitive* p ){ primitives.push_back( p ); return primitives.size(); };
	std::vector<Primitive*>* getPrimitives( void ){ return &primitives; };
	void setShader( Shader* shader ){ this-> shader = shader; };
	Shader* getShader( void );
	virtual void writeOff( FILE* fp ) = 0;
};

class Geometry_Generic: public Geometry
{
	public:
	Geometry_Generic( void ){ };
	Geometry_Generic( Primitive* p ){ primitives.push_back( p ); };
	void writeOff( FILE* fp );
};
class Geometry_AAB: public Geometry
{
	public:
	Geometry_AAB( void ){ };
	Geometry_AAB( Primitive* p ){ primitives.push_back( p ); };
	void writeOff( FILE* fp );
};
class Geometry_AAP: public Geometry
{
	public:
	Geometry_AAP( void ){ };
	Geometry_AAP( Primitive* p ){ primitives.push_back( p ); };
	void writeOff( FILE* fp );
};

class Sphere: public Primitive
{
	public:
	Sphere_struct s; 
	Sphere( float3 c, float r ){ s.radius = r; s.center = c; type = Sphere_t; };
};

class Triangle: public Primitive
{
	public:
	Triangle_struct s; 
	Triangle( float3 v1, float3 v2, float3 v3 ){ s.v1 = v1; s.v2 = v2; s.v3 = v3; type = Triangle_t; };
};

class Rectangle: public Primitive
{
	public:
	Rectangle_struct s;
	Rectangle( float3 bmin, float3 s1, float3 s2 ){ s.bmin = bmin; s.s1 = s1; s.s2 = s2; type = Rectangle_t; };
};

class Quadrilateral: public Primitive
{
	public:
	Quadrilateral_struct s;
	Quadrilateral( float3 v1, float3 v2, float3 v3, float3 v4 ){ s.v1 = v1; s.v2 = v2; s.v3 = v3; s.v4 = v4; type = Quadrilateral_t; };
};

class AAB: public Primitive
{
	public:
	AAB_struct s;
	AAB( float3 bmin, float3 bmax ){ s.bmin = bmin; s.bmax = bmax; type = AAB_t; };
};

class Box: public Primitive
{
	public:
	Box_struct s;
	Box( float3 s1, float3 s2, float3 s3, float h ){ s.s1 = s1; s.s2 = s2; s.s3 = s3; s.h = h; type = Box_t; };
};

class Pyramid: public Geometry
{
	public:
	Pyramid( float3 v1, float3 v2, float3 v3, float3 v4, float3 apex );
};


Geometry* make_sphere( float3 c, float r );
Geometry* make_triangle( float3 v1, float3 v2, float3 v3 );
Geometry* make_rectangle( float3 bmin, float3 s1, float3 s2 );
Geometry* make_quadrilateral( float3 v1, float3 v2, float3 v3, float3 v4 );
Geometry* make_AAB( float3 bmin, float3 bmax );
Geometry* make_box( float3 s1, float3 s2, float3 s3, float h );
Geometry* make_AAPrism( float3 v1, float3 v2, float3 v5 );

// These function make standard versions of the various objects, for shorthand.
Geometry* make_AAB1( float3 bmin );
Geometry* make_AAB1s( float3 bmin, float scale );

Geometry* make_AAPrism_UN( float3 bmin, float h );
Geometry* make_AAPrism_US( float3 bmin, float h );
Geometry* make_AAPrism_UE( float3 bmin, float h );
Geometry* make_AAPrism_UW( float3 bmin, float h );

Geometry* make_AAPrism_DN( float3 bmin, float h );
Geometry* make_AAPrism_DE( float3 bmin, float h );
Geometry* make_AAPrism_DS( float3 bmin, float h );
Geometry* make_AAPrism_DW( float3 bmin, float h );

Geometry* make_AAPrism_NW( float3 bmin, float h );
Geometry* make_AAPrism_SW( float3 bmin, float h );
Geometry* make_AAPrism_SE( float3 bmin, float h );
Geometry* make_AAPrism_NE( float3 bmin, float h );
#endif
#ifndef geometry_h
#define geometry_h

#include <vector>
#include <stdio.h>
#include "vtypes.h"
#include "shaders.h"
#include "shared.h"
enum PrimitiveType { SPHERE, TRIANGLE, QUADRILATERAL, AAB, BOX};
enum GeometryType { GEO_GENERIC=0, GEO_AAB=1, GEO_AAP=2  };
enum Orientation { UN, UW, US, UE, DN, DW, DS, DE, NW, SW, SE, NE };

class AAB_t;

class Primitive
{
	protected:
	PrimitiveType type;
	public:
	PrimitiveType getType( void ) { return type; };
	virtual ~Primitive( void ) = default;
	virtual AAB_t bound() = 0;
};


class AAB_t: public Primitive
{
	public:
	AAB_s s;
	AAB_t( ){ s.bmin = make_float3(0.f); s.bmax = make_float3(0.f); type = AAB; };
	AAB_t( float3 bmin, float3 bmax ){
		float3 diff = bmax-bmin;
		if( diff.x < 0.01f )
		{
			bmin.x -= 0.005f;
			bmax.x += 0.005f;
		}
		if( diff.y < 0.01f )
		{
			bmin.y -= 0.005f;
			bmax.y += 0.005f;
		}
		if( diff.z < 0.01f )
		{
			bmin.z -= 0.005f;
			bmax.z += 0.005f;
		}
		s.bmin = bmin;
		s.bmax = bmax;
		type = AAB;
	};
	
	float3 min(){ return s.bmin; }
	float3 max(){ return s.bmax; }
	AAB_t merge( AAB_t other )
	{
		return AAB_t( fminf( min(), other.min() ), fmaxf( max(), other.max() ) );
	}
	
	virtual AAB_t bound()
	{
		return *this;
	}
};

class Box_t: public Primitive
{
	public:
	Box s;
	Box_t( float3 s1, float3 s2, float3 s3, float h ){ s.s1 = s1; s.s2 = s2; s.s3 = s3; s.h = h; type = BOX; };
	virtual AAB_t bound()
	{
		return AAB_t(
			fminf( s.s1, s.s2, s.s3 ),
			fmaxf( s.s1, s.s2, s.s3 )
		);
	}
};

class Sphere_t: public Primitive
{
	public:
	Sphere s; 
	Sphere_t( float3 c, float r ){ s.radius = r; s.center = c; type = SPHERE; };
	virtual AAB_t bound()
	{
		return AAB_t(
			s.center - s.radius,
			s.center + s.radius
		);
	}
};

class Triangle_t: public Primitive
{
	public:
	Triangle s; 
	Triangle_t( float3 v1, float3 v2, float3 v3 ){ s.v1 = v1; s.v2 = v2; s.v3 = v3; type = TRIANGLE; };
	virtual AAB_t bound()
	{
		return AAB_t(
			fminf( s.v1, fminf(s.v2, s.v3) ),
			fmaxf( s.v1, fmaxf(s.v2, s.v3) )
		);
	}
};

class Quadrilateral_t: public Primitive
{
	public:
	Quadrilateral s;
	Quadrilateral_t( float3 v1, float3 v2, float3 v3, float3 v4 ){ s.v1 = v1; s.v2 = v2; s.v3 = v3; s.v4 = v4; type = QUADRILATERAL; };
	virtual AAB_t bound()
	{
		return AAB_t(
			fminf( fminf( s.v1, s.v2 ), fminf( s.v3, s.v4 ) ),
			fmaxf( fmaxf( s.v1, s.v2 ), fmaxf( s.v3, s.v4 ) )
		);
	}
};


class Geometry
{
	Shader* shader;
	protected:
	bool dirty;
	std::vector<Primitive*> primitives;
	
	virtual void buildPrimitives( void ){};
	
	public:
	
	Geometry( void ){ shader = NULL; dirty = true; };
	~Geometry( void ){ primitives.clear( ); }
	int addPrimitive( Primitive* p ){ primitives.push_back( p ); return primitives.size(); };
	void clearPrimitives( void );
	std::vector<Primitive*>* getPrimitives( void );
	void setShader( Shader* shader ){ this-> shader = shader; };
	Shader* getShader( void );
	
	virtual void move( float3 d ) = 0;
	void move( float dx, float dy, float dz ){ move( make_float3( dx, dy, dz ) ); };
	virtual void writeOff( FILE* fp ) = 0;
	virtual void select( void ){};
	virtual void deselect( void ){};
};

class Geometry_Generic: public Geometry
{
	protected:
	virtual void buildPrimitives( void ){};
	
	public:
	Geometry_Generic( void ){ };
	Geometry_Generic( Primitive* p ){ primitives.push_back( p ); };
	virtual void move( float3 d ){};
	void writeOff( FILE* fp );
};
class Geometry_AAB: public Geometry
{
	float3 bmin;
	float3 bmax_offset;
	
	protected:
	virtual void buildPrimitives( void );
	
	public:
	Geometry_AAB( void ){ };
	Geometry_AAB( float3 bmin ){ this->bmin = bmin; bmax_offset = make_float3( 1.f ); };
	Geometry_AAB( float3 bmin, float3 bmax ){ this->bmin = bmin; this->bmax_offset = bmax - bmin; };
	virtual void move( float3 d );
	virtual void deselect( void );
	void writeOff( FILE* fp );
};
class Geometry_AAP: public Geometry
{
	float3 bmin;
	Orientation o;
	void make_AAPrismFlip( float3 v1, float3 v2, float3 v5 );
	void make_AAPrism( float3 v1, float3 v2, float3 v5 );
	
	protected:
	virtual void buildPrimitives( void );
	
	public:
	Geometry_AAP( void ){ };
	Geometry_AAP( float3 bmin, Orientation o );
	void writeOff( FILE* fp );
	virtual void move( float3 d );
	virtual void deselect( void );
};

class Pyramid: public Geometry
{
	float3 v1, v2, v3, v4, apex;
	
	protected:
	virtual void buildPrimitives( void ){};
	
	public:
	Pyramid( float3 v1, float3 v2, float3 v3, float3 v4, float3 apex );
	void writeOff( FILE* fp );
	virtual void move( float3 d ){};
};


Geometry* make_sphere( float3 c, float r );
Geometry* make_triangle( float3 v1, float3 v2, float3 v3 );
Geometry* make_quadrilateral( float3 v1, float3 v2, float3 v3, float3 v4 );
Geometry* make_box( float3 s1, float3 s2, float3 s3, float h );

/*
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
*/
#endif
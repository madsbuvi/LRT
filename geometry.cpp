#include "geometry.h"

Shader* Geometry::getShader( void ) {
	if ( shader == NULL ){
		return Shader::getDefaultShader();
	}
	else{
		return shader;
	}
}


void Geometry_Generic::writeOff( FILE* fp )
{
}
void Geometry_AAB::writeOff( FILE* fp )
{
}
void Geometry_AAP::writeOff( FILE* fp )
{
}



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

Geometry* make_sphere( float3 c, float r )
{
	Sphere* p = new Sphere( c, r );
	return new Geometry_Generic( p );
}

Geometry* make_triangle( float3 v1, float3 v2, float3 v3 )
{
	Triangle* p = new Triangle( v1, v2, v3 );
	return new Geometry_Generic( p );
}

Geometry* make_rectangle( float3 bmin, float3 s1, float3 s2 )
{
	Rectangle* p = new Rectangle( bmin, s1, s2 );
	return new Geometry_Generic( p );
}

Geometry* make_quadrilateral( float3 v1, float3 v2, float3 v3, float3 v4 )
{
	Quadrilateral* p = new Quadrilateral( v1, v2, v3, v4 );
	return new Geometry_Generic( p );
}

Geometry* make_AAB( float3 bmin, float3 bmax )
{
	AAB* p = new AAB( bmin, bmax );
	return new Geometry_Generic( p );
}

Geometry* make_box( float3 s1, float3 s2, float3 s3, float h )
{
	Box* p = new Box( s1, s2, s3, h );
	return new Geometry_Generic( p );
}


Geometry* make_AAPrism( float3 v1, float3 v2, float3 v5 )
{
	float3 v3, v4, v6;
	Geometry* g = new Geometry_AAP();
	v3 = make_float3( v5.x, v1.y, v5.z );
	v4 = v3 + v2 - v1;
	v6 = make_float3( v4.x, v5.y, v4.z );
	Triangle* s1 = new Triangle( v1, v3, v5 );
	g->addPrimitive( s1 );
	Triangle* s2 = new Triangle( v2, v4, v6 );
	g->addPrimitive( s2 );
	
	Triangle* r1 = new Triangle( v1, v2, v5 );
	g->addPrimitive( r1 );
	Triangle* r2 = new Triangle( v2, v5, v6 );
	g->addPrimitive( r2 );
	
	Triangle* b1 = new Triangle( v1, v2, v3 );
	g->addPrimitive( b1 );
	Triangle* b2 = new Triangle( v2, v3, v4 );
	g->addPrimitive( b2 );
	Triangle* w1 = new Triangle( v3, v4, v5 );
	g->addPrimitive( w1 );
	Triangle* w2 = new Triangle( v4, v5, v6 );
	g->addPrimitive( w2 );
	return g;
}
Geometry* make_AAB1( float3 bmin )
{
	AAB* p = new AAB( bmin, make_float3( bmin.x+1.f, bmin.y+1.f, bmin.z+1.f ) );
	return new Geometry_Generic( p );
}
Geometry* make_AAB1s( float3 bmin, float scale )
{
	AAB* p = new AAB( bmin, make_float3( bmin.x+scale, bmin.y+scale, bmin.z+scale ) );
	return new Geometry_Generic( p );
}

Geometry* make_AAPrism_UN( float3 bmin, float h )
{
	return make_AAPrism( make_float3( bmin.x+1.f, bmin.y, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y, bmin.z ), make_float3( bmin.x, bmin.y+h, bmin.z+1.f ) );
}

Geometry* make_AAPrism_US( float3 bmin, float h )
{
	return make_AAPrism( bmin, make_float3( bmin.x, bmin.y, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y+h, bmin.z ) );
}

Geometry* make_AAPrism_UE( float3 bmin, float h )
{
	return make_AAPrism( make_float3( bmin.x, bmin.y, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y, bmin.z+1.f ), make_float3( bmin.x, bmin.y+h, bmin.z ) );
}

Geometry* make_AAPrism_UW( float3 bmin, float h )
{
	return make_AAPrism( make_float3( bmin.x+1.f, bmin.y, bmin.z), bmin, make_float3( bmin.x+1.f, bmin.y+h, bmin.z+1.f ) );
}


Geometry* make_AAPrism_DN( float3 bmin, float h )
{
	return make_AAPrism( make_float3( bmin.x+1.f, bmin.y+h, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y+h, bmin.z ), make_float3( bmin.x, bmin.y, bmin.z+1.f ) );
}

Geometry* make_AAPrism_DS( float3 bmin, float h )
{
	return make_AAPrism( make_float3( bmin.x, bmin.y+h, bmin.z ), make_float3( bmin.x, bmin.y+h, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y, bmin.z ) );
}

Geometry* make_AAPrism_DE( float3 bmin, float h )
{
	return make_AAPrism( make_float3( bmin.x, bmin.y+h, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y+h, bmin.z+1.f ), bmin );
}

Geometry* make_AAPrism_DW( float3 bmin, float h )
{
	return make_AAPrism( make_float3( bmin.x+1.f, bmin.y+h, bmin.z), make_float3( bmin.x, bmin.y+h, bmin.z ), make_float3( bmin.x+1.f, bmin.y, bmin.z+1.f ) );
}



Geometry* make_AAPrismFlip( float3 v1, float3 v2, float3 v5 )
{
	float3 v3, v4, v6;
	Geometry* g = new Geometry_AAP();
	v3 = make_float3( v1.x, v5.y, v5.z );
	v4 = v3 + v2 - v1;
	v6 = make_float3( v5.x, v4.y, v4.z );
	Triangle* s1 = new Triangle( v1, v3, v5 );
	g->addPrimitive( s1 );
	Triangle* s2 = new Triangle( v2, v4, v6 );
	g->addPrimitive( s2 );
	
	Triangle* r1 = new Triangle( v1, v2, v5 );
	g->addPrimitive( r1 );
	Triangle* r2 = new Triangle( v2, v5, v6 );
	g->addPrimitive( r2 );
	
	Triangle* b1 = new Triangle( v1, v2, v3 );
	g->addPrimitive( b1 );
	Triangle* b2 = new Triangle( v2, v3, v4 );
	g->addPrimitive( b2 );
	Triangle* w1 = new Triangle( v3, v4, v5 );
	g->addPrimitive( w1 );
	Triangle* w2 = new Triangle( v4, v5, v6 );
	g->addPrimitive( w2 );
	
	return g;
}


Geometry* make_AAPrism_NW( float3 bmin, float h )
{
	return make_AAPrismFlip( make_float3( bmin.x, bmin.y, bmin.z), make_float3( bmin.x, bmin.y+1.f, bmin.z ), make_float3( bmin.x+1.f, bmin.y, bmin.z+h ) );
}

Geometry* make_AAPrism_SW( float3 bmin, float h )
{
	return make_AAPrismFlip( make_float3( bmin.x+1.f, bmin.y, bmin.z ), make_float3( bmin.x+1.f, bmin.y+1.f, bmin.z ), make_float3( bmin.x, bmin.y, bmin.z+h ) );
}

Geometry* make_AAPrism_SE( float3 bmin, float h )
{
	return make_AAPrismFlip( make_float3( bmin.x+1.f, bmin.y, bmin.z+h ), make_float3( bmin.x+1.f, bmin.y+1.f, bmin.z+h ), make_float3( bmin.x, bmin.y, bmin.z ) );
}

Geometry* make_AAPrism_NE( float3 bmin, float h )
{
	return make_AAPrismFlip( make_float3( bmin.x, bmin.y, bmin.z+h), make_float3( bmin.x, bmin.y+1.f, bmin.z+h ), make_float3( bmin.x+1.f, bmin.y, bmin.z ) );
}
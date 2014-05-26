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
void Pyramid::writeOff( FILE* fp )
{
}

void Geometry::clearPrimitives( void ){
	for( Primitive* p: primitives )
	{
		p->~Primitive();
	}
	primitives.clear( );
}



Pyramid::Pyramid( float3 v1, float3 v2, float3 v3, float3 v4, float3 apex ){
	Quadrilateral_t* base = new Quadrilateral_t( v1, v2, v3, v4 );
	Triangle_t* s1 = new Triangle_t( v1, v2, apex );
	Triangle_t* s2 = new Triangle_t( v2, v3, apex );
	Triangle_t* s3 = new Triangle_t( v3, v4, apex );
	Triangle_t* s4 = new Triangle_t( v4, v1, apex );
	addPrimitive( base );
	addPrimitive( s1 );
	addPrimitive( s2 );
	addPrimitive( s3 );
	addPrimitive( s4 );
}

Geometry* make_sphere( float3 c, float r )
{
	Sphere_t* p = new Sphere_t( c, r );
	return new Geometry_Generic( p );
}

Geometry* make_triangle( float3 v1, float3 v2, float3 v3 )
{
	Triangle_t* p = new Triangle_t( v1, v2, v3 );
	return new Geometry_Generic( p );
}

Geometry* make_quadrilateral( float3 v1, float3 v2, float3 v3, float3 v4 )
{
	Quadrilateral_t* p = new Quadrilateral_t( v1, v2, v3, v4 );
	return new Geometry_Generic( p );
}

Geometry* make_box( float3 s1, float3 s2, float3 s3, float h )
{
	Box_t* p = new Box_t( s1, s2, s3, h );
	return new Geometry_Generic( p );
}

void Geometry_AAP::make_AAPrismFlip( float3 v1, float3 v2, float3 v5 )
{
	float3 v3, v4, v6;
	v3 = make_float3( v1.x, v5.y, v5.z );
	v4 = v3 + v2 - v1;
	v6 = make_float3( v5.x, v4.y, v4.z );
	Triangle_t* s1 = new Triangle_t( v1, v3, v5 );
	addPrimitive( s1 );
	Triangle_t* s2 = new Triangle_t( v2, v4, v6 );
	addPrimitive( s2 );
	
	Triangle_t* r1 = new Triangle_t( v1, v2, v5 );
	addPrimitive( r1 );
	Triangle_t* r2 = new Triangle_t( v2, v5, v6 );
	addPrimitive( r2 );
	
	Triangle_t* b1 = new Triangle_t( v1, v2, v3 );
	addPrimitive( b1 );
	Triangle_t* b2 = new Triangle_t( v2, v3, v4 );
	addPrimitive( b2 );
	Triangle_t* w1 = new Triangle_t( v3, v4, v5 );
	addPrimitive( w1 );
	Triangle_t* w2 = new Triangle_t( v4, v5, v6 );
	addPrimitive( w2 );
}

void Geometry_AAP::make_AAPrism( float3 v1, float3 v2, float3 v5 )
{
	float3 v3, v4, v6;
	v3 = make_float3( v5.x, v1.y, v5.z );
	v4 = v3 + v2 - v1;
	v6 = make_float3( v4.x, v5.y, v4.z );
	Triangle_t* s1 = new Triangle_t( v1, v3, v5 );
	addPrimitive( s1 );
	Triangle_t* s2 = new Triangle_t( v2, v4, v6 );
	addPrimitive( s2 );
	
	Triangle_t* r1 = new Triangle_t( v1, v2, v5 );
	addPrimitive( r1 );
	Triangle_t* r2 = new Triangle_t( v2, v5, v6 );
	addPrimitive( r2 );
	
	Triangle_t* b1 = new Triangle_t( v1, v2, v3 );
	addPrimitive( b1 );
	Triangle_t* b2 = new Triangle_t( v2, v3, v4 );
	addPrimitive( b2 );
	Triangle_t* w1 = new Triangle_t( v3, v4, v5 );
	addPrimitive( w1 );
	Triangle_t* w2 = new Triangle_t( v4, v5, v6 );
	addPrimitive( w2 );
}

std::vector<Primitive*>* Geometry::getPrimitives( void )
{
	//printf("ConvokeB\n");
	if(dirty)
	{
		buildPrimitives();
		dirty = false;
	}
	return &primitives;
}
void Geometry_AAB::buildPrimitives( void )
{
	primitives.clear();
	addPrimitive( new AAB_t( bmin, bmin + bmax_offset ) );
}

void Geometry_AAB::move( float3 d )
{
	bmin += d;
	dirty = true;
}
void Geometry_AAB::deselect( void )
{
	bmin = floor(bmin + 0.5f);
	
	dirty = true;
}

void Geometry_AAP::move( float3 d )
{
	bmin += d;
	dirty = true;
}
void Geometry_AAP::deselect( void )
{
	bmin = floor(bmin + 0.5f);
	
	dirty = true;
}

void Geometry_AAP::buildPrimitives( void )
{
	primitives.clear();
	switch( o )
	{
		case UN:
			make_AAPrism( make_float3( bmin.x+1.f, bmin.y, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y, bmin.z ), make_float3( bmin.x, bmin.y+1.f, bmin.z+1.f ) );
			break;
		case UW:
			make_AAPrism( make_float3( bmin.x+1.f, bmin.y, bmin.z), bmin, make_float3( bmin.x+1.f, bmin.y+1.f, bmin.z+1.f ) );
			break;
		case US:
			make_AAPrism( bmin, make_float3( bmin.x, bmin.y, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y+1.f, bmin.z ) );
			break;
		case UE:
			make_AAPrism( make_float3( bmin.x, bmin.y, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y, bmin.z+1.f ), make_float3( bmin.x, bmin.y+1.f, bmin.z ) );
			break;
		case DN:
			make_AAPrism( make_float3( bmin.x+1.f, bmin.y+1.f, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y+1.f, bmin.z ), make_float3( bmin.x, bmin.y, bmin.z+1.f ) );
			break;
		case DW:
			make_AAPrism( make_float3( bmin.x+1.f, bmin.y+1.f, bmin.z), make_float3( bmin.x, bmin.y+1.f, bmin.z ), make_float3( bmin.x+1.f, bmin.y, bmin.z+1.f ) );
			break;
		case DS:
			make_AAPrism( make_float3( bmin.x, bmin.y+1.f, bmin.z ), make_float3( bmin.x, bmin.y+1.f, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y, bmin.z ) );
			break;
		case DE:
			make_AAPrism( make_float3( bmin.x, bmin.y+1.f, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y+1.f, bmin.z+1.f ), bmin );
			break;
		case NW:
			make_AAPrismFlip( make_float3( bmin.x, bmin.y, bmin.z), make_float3( bmin.x, bmin.y+1.f, bmin.z ), make_float3( bmin.x+1.f, bmin.y, bmin.z+1.f ) );
			break;
		case SW:
			make_AAPrismFlip( make_float3( bmin.x+1.f, bmin.y, bmin.z ), make_float3( bmin.x+1.f, bmin.y+1.f, bmin.z ), make_float3( bmin.x, bmin.y, bmin.z+1.f ) );
			break;
		case SE:
			make_AAPrismFlip( make_float3( bmin.x+1.f, bmin.y, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y+1.f, bmin.z+1.f ), make_float3( bmin.x, bmin.y, bmin.z ) );
			break;
		case NE:
			make_AAPrismFlip( make_float3( bmin.x, bmin.y, bmin.z+1.f), make_float3( bmin.x, bmin.y+1.f, bmin.z+1.f ), make_float3( bmin.x+1.f, bmin.y, bmin.z ) );
			break;
	}
}

Geometry_AAP::Geometry_AAP( float3 bmin, Orientation o )
{
	this->bmin = bmin;
	this->o = o;
}
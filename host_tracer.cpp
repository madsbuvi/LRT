#include "host_tracer.h"
#define fmaxf fmaxf3
#define fminf fminf3
#include "intersectors.inc"

int selectTrace( Ray ray, std::vector<Geometry*>& geometry )
{

	Result closest = MISS;
	int closest_i = -1;
	closest.t = RT_DEFAULT_MAX+1.f;
	Result nyligst;
	
	for( unsigned i = 0; i < geometry.size(); i++ )
	{
		Geometry* geo = geometry[i];
		for( Primitive* prim: *geo->getPrimitives() )
		{
			switch( prim->getType() )
			{
				case SPHERE:
				{
					nyligst = intersect_sphere( ray, dynamic_cast<Sphere_t*>(prim)->s );
					break;
				}	
				case TRIANGLE:
				{
					Triangle tr = dynamic_cast<Triangle_t*>(prim)->s;
					nyligst = intersect_triangle( ray, tr.v1, tr.v2, tr.v3 );
					break;
				}
					
				case AAB:
				{
					AAB_s aab = dynamic_cast<AAB_t*>(prim)->s;
					nyligst = intersect_AAB( ray, aab.bmin, aab.bmax );
					break;
				}
				case BOX:
				{
					Box box = dynamic_cast<Box_t*>(prim)->s;

					nyligst = intersect_box( ray, box );
					
					break;
				}
			}
			

			if( nyligst.t > RT_DEFAULT_MIN && nyligst.t < closest.t )
			{
				closest = nyligst;
				closest_i = i;
			}
			
		}
	}
	return closest_i;
}
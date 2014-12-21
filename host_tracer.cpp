#include "host_tracer.h"
#define fmaxf fmaxf3
#define fminf fminf3
#include "intersectors.inc"



int selectTrace( Ray ray, int* bvh_ints, float* bvh_floats, int )
{

	
	// RAY TRACING
	Result closest = MISS;
	int closest_i = -1;
	closest.t = RT_DEFAULT_MAX+1.f;
	Result nyligst;
	//printi( nobjects );
	
	int id = 0;
	
	while( id >= 0 )
	{
		int type = bvh_ints[ id*6 + 0 ];
		int index = bvh_ints[ id*6 + 1 ];
		int shader = bvh_ints[ id*6 + 2 ];
		int shaderIndex = bvh_ints[ id*6 + 3 ];
		int next = bvh_ints[ id*6 + 4 ];
		int failureLink = bvh_ints[ id*6 + 5 ];
		
		float t = bound(
			ray,
			make_float3( bvh_floats[ id*6 + 0 ], bvh_floats[ id*6 + 1 ], bvh_floats[ id*6 + 2 ] ),
			make_float3( bvh_floats[ id*6 + 3 ], bvh_floats[ id*6 + 4 ], bvh_floats[ id*6 + 5 ] )
			);
			

		fprintf( stdout,  "Processing[%d ] < %.4g, %.4g, %.4g > , < %.4g, %.4g, %.4g >\n", id,
			bvh_floats[ id*6 + 0 ], bvh_floats[ id*6 + 1 ], bvh_floats[ id*6 + 2 ], bvh_floats[ id*6 + 3 ], bvh_floats[ id*6 + 4 ], bvh_floats[ id*6 + 5 ] );
		fprintf( stdout,  "\ttype: %d\n", type );
		fprintf( stdout,  "\tindex: %d\n", index );
		fprintf( stdout,  "\tshader: %d\n", shader );
		fprintf( stdout,  "\tshaderIndex: %d\n", shaderIndex );
		fprintf( stdout,  "\tnext: %d\n", next );
		fprintf( stdout,  "\tfailureLink: %d\n", failureLink );
		
		if( t > RT_DEFAULT_MIN && t < closest.t )
		{
			// Node is relevant
			
			if( type >= 0 )
			{
				printf( "Reached a primitive: [ %d, %d ]\n", index, type );
				
				printf( "Continuing to next node: %d -> %d\n", id, next);
				id = next;
			}
			else
			{
				// Internal node, continue exploring the tree
				printf( "Continuing to next node: %d -> %d\n", id, next);
				id = next;
			}
		}
		else
		{
			//Follow failure link
				printf( "Following failure link: %d -> %d\n", id, failureLink);
			id = failureLink;
		}
		
	}
	return 0;
}

int selectTrace2( Ray ray, std::vector<Geometry*>& geometry )
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
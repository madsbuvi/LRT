
static Result intersect(
	rt_context context,
	Ray ray
	)
{
	// RAY TRACING
	Result closest = MISS;
	closest.t = RT_DEFAULT_MAX+1.f;
	Result nyligst;
	//printi( nobjects );
	
	for( int i = 0; i < context.nobjects; i++ )
	{
		__global int* p = &context.primitives[ context.geometry[ i*4 ] ];
		int nprim = context.geometry[ i*4 + 1];
		for( int j = 0; j < nprim; j++ )
		{
			int type = p[ j*2 ];
			int index = p[ j*2 + 1 ];
			
			
			switch( type )
			{
				case Sphere_t:
				{
					Sphere sphere = { context.sphere_centers[index], context.sphere_radi[index] };
					nyligst = intersect_sphere( ray, sphere );
					break;
				}	
				case Triangle_t:
					nyligst = intersect_triangle( ray, context.triangles[index*3], context.triangles[index*3+1], context.triangles[index*3+2] );
					break;
					
				case AAB_t:
					nyligst = intersect_AAB( ray, context.AABs[ index*2 ], context.AABs[ index*2 + 1 ] );
					break;
				case Box_t:
				{
					Box box = {
						context.boxes[ index*3 + 0 ],
						context.boxes[ index*3 + 1 ],
						context.boxes[ index*3 + 2 ],
						context.box_heights[ index ]
					};

					nyligst = intersect_box( ray, box );
					
					break;
				}
			}

			if( nyligst.t > RT_DEFAULT_MIN && nyligst.t < closest.t )
			{
				closest = nyligst;
				closest.shader = i;
				ifprint("REGULAR New closest at <%d, %d, %d>\n", type, index, i);
			}
			
		}
	}
	return closest;
}

static Result bvhintersect(
	rt_context context,
	Ray ray
	)
{
	// RAY TRACING
	Result closest = MISS;
	closest.t = RT_DEFAULT_MAX+1.f;
	Result nyligst;
	//printi( nobjects );
	
	int id = 0;
	
	while( id >= 0 )
	{
		ifprint("\nEntering new id[%d]\n", id);
		int type = context.bvh_ints[ id*6 + 0 ];
		int index = context.bvh_ints[ id*6 + 1 ];
		int shader = context.bvh_ints[ id*6 + 2 ];
		int shaderIndex = context.bvh_ints[ id*6 + 3 ];
		int next = context.bvh_ints[ id*6 + 4 ];
		int failureLink = context.bvh_ints[ id*6 + 5 ];
		
		
		ifprint("type: %d\n", type);
		ifprint("index: %d\n", index);
		ifprint("shader: %d\n", shader);
		ifprint("shaderIndex: %d\n", shaderIndex);
		ifprint("next: %d\n", next);
		ifprint("failureLink: %d\n", failureLink);
		
		float t = bound(
			ray,
			make_float3( context.bvh_floats[ id*6 + 0 ], context.bvh_floats[ id*6 + 1 ], context.bvh_floats[ id*6 + 2 ] ),
			make_float3( context.bvh_floats[ id*6 + 3 ], context.bvh_floats[ id*6 + 4 ], context.bvh_floats[ id*6 + 5 ] )
			);
			
		float3 bmin = make_float3( context.bvh_floats[ id*6 + 0 ], context.bvh_floats[ id*6 + 1 ], context.bvh_floats[ id*6 + 2 ] );
		float3 bmax = make_float3( context.bvh_floats[ id*6 + 3 ], context.bvh_floats[ id*6 + 4 ], context.bvh_floats[ id*6 + 5 ] );
		ifprint("bmin: <%.4g, %.4g, %.4g>\n", bmin.x, bmin.y, bmin.z);
		ifprint("bmax: <%.4g, %.4g, %.4g>\n", bmax.x, bmax.y, bmax.z);
		ifprint("t: %g\n", t);
		if( t < closest.t )
		{
			// Node is relevant
			
			if( type >= 0 )
			{
				// Leaf node
				// Test the primitive
				switch( type )
				{
					case Sphere_t:
					{
						Sphere sphere = { context.sphere_centers[index], context.sphere_radi[index] };
						nyligst = intersect_sphere( ray, sphere );
						break;
					}	
					case Triangle_t:
						nyligst = intersect_triangle( ray, context.triangles[index*3], context.triangles[index*3+1], context.triangles[index*3+2] );
						break;
						
					case AAB_t:
						nyligst = intersect_AAB( ray, context.AABs[ index*2 ], context.AABs[ index*2 + 1 ] );
						break;
					case Box_t:
					{
						Box box = {
							context.boxes[ index*3 + 0 ],
							context.boxes[ index*3 + 1 ],
							context.boxes[ index*3 + 2 ],
							context.box_heights[ index ]
						};

						nyligst = intersect_box( ray, box );
						
						break;
					}
				}

				if( nyligst.t > RT_DEFAULT_MIN && nyligst.t < closest.t )
				{
					closest = nyligst;
					closest.shader = shader;
					ifprint("New closest at <%d, %d, %d>\n", type, index, shader);
				}
				
				id = next;
			}
			else
			{
				// Internal node, continue exploring the tree
				id = next;
			}
		}
		else
		{
			//Follow failure link
			id = failureLink;
		}
		
	}
	return closest;
}
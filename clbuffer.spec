/***********************************************
 *
 *	Specification of the various buffers used in raytracer.cl
 *	
 *	All buffers contain only base types such as int and float
 *	The specification explains the member data for each buffer
 *	The buffers contain these members grouped linearly as if they were structs as described below
 *
 ***********************************************/


BVH_FLOATS = 
{
	AAB_s
};

BVH_INTS = 
{
	// Primitive type enum, negative for internal nodes
	type,
	// Index of the primitive, not set for internal nodes
	index,
	// Shader used by the primitive, not set for internal nodes
	shader,
	// Index to shader data used by the primitive, not set for internal nodes
	shader index
	// Index to the next bvh node to examine (if no failure)
	next,
	// Index to the next bvh node to examine (if failure)
	failure link
}
	
	if( ! m_Leaf )
	{
		ints[ m_Id*4 + 0 ] = -1;
		ints[ m_Id*4 + 4 ] = m_Second->id();
		ints[ m_Id*4 + 5 ] = m_FailureLink;
		m_First->write( ints, floats );
		m_Second->write( ints, floats );
	}
	else
	{
		ints[ m_Id*4 + 0 ] = m_Data.type;
		ints[ m_Id*4 + 1 ] = m_Data.index;
		ints[ m_Id*4 + 2 ] = m_Data.shader;
		ints[ m_Id*4 + 3 ] = m_Data.shaderIndex;
		ints[ m_Id*4 + 4 ] = m_FailureLink;
		ints[ m_Id*4 + 5 ] = m_FailureLink;
	}
#include <algorithm>
#include <stdio.h>
#include "bvh.h"


namespace BVH
{
KP::KP( Primitives primitives )
	: m_Size(0)
{
	root = KPNode::pointer( new KPNode( primitives, KPNode::X_Axis, m_Size, -1 ) );
}

KP::pointer KP::build( Primitives primitives )
{
	return pointer( new KP( primitives ) );
}

static bool compareX( Primdata i, Primdata j )
{
	return ( i.bound.min().x + i.bound.max().x ) / 2 < ( j.bound.min().x + j.bound.max().x ) / 2;
}
static bool compareY( Primdata i, Primdata j )
{
	return ( i.bound.min().y + i.bound.max().y ) / 2 < ( j.bound.min().y + j.bound.max().y ) / 2;
}
static bool compareZ( Primdata i, Primdata j )
{
	return ( i.bound.min().z + i.bound.max().z ) / 2 < ( j.bound.min().z + j.bound.max().z ) / 2;
}
	
	
KPNode::KPNode( Primitives primitives, Axis axis, int& id, int failureLink )
	: m_Id(id), m_FailureLink(failureLink)
{
	id++;
	if( !primitives.size() )
	{
		// Should not happen
		return;
	}
	
	if( primitives.size() == 1 )
	{
		m_Data = primitives[0];
		m_Leaf = true;
		return;
	}
	
	m_Leaf = false;
	Axis nextAxis;
	
	switch(axis)
	{
		case X_Axis:
			std::sort( primitives.begin(), primitives.end(), compareX );
			nextAxis = Y_Axis;
			break;
			
		case Y_Axis:
			std::sort( primitives.begin(), primitives.end(), compareY );
			nextAxis = Z_Axis;
			break;
			
		case Z_Axis:
			std::sort( primitives.begin(), primitives.end(), compareZ );
			nextAxis = X_Axis;
			break;
	}
	
	m_Data = primitives[0];
	
	for( Primdata& data: primitives )
	{
		m_Data.bound = m_Data.bound.merge( data.bound );
	}
	
	m_First = pointer( new KPNode( Primitives( primitives.begin(), primitives.begin() + primitives.size()/2 ), nextAxis, id, m_FailureLink ) );
	m_Second = pointer( new KPNode( Primitives( primitives.begin() + primitives.size()/2, primitives.end() ), nextAxis, id, m_First->id() ) );
	
	/*
	fprintf( stdout,  "Buildt[%d] < %.4g, %.4g, %.4g > , < %.4g, %.4g, %.4g >\n", m_Id,
		m_Data.bound.min().x, m_Data.bound.min().y, m_Data.bound.min().z, m_Data.bound.max().x, m_Data.bound.max().y, m_Data.bound.max().z );
	fprintf( stdout,  "\ttype: %d\n", m_Data.type );
	fprintf( stdout,  "\tindex: %d\n", m_Data.index );
	fprintf( stdout,  "\tshader: %d\n", m_Data.shader );
	fprintf( stdout,  "\tshaderIndex: %d\n", m_Data.shaderIndex );
	if( !m_Leaf ) fprintf( stdout,  "\tnext: %d\n", m_Second->id() );
	fprintf( stdout,  "\tfailureLink: %d\n", failureLink );
	*/
}

void BVH::write( int* ints, float* floats )
{
	root->write( ints, floats );

}

void KPNode::write( int* ints, float* floats )
{

	//fprintf( stdout,  "[ %d ] < %.4g, %.4g, %.4g > , < %.4g, %.4g, %.4g >\n", m_Id,
	//	m_Data.bound.min().x, m_Data.bound.min().y, m_Data.bound.min().z, m_Data.bound.max().x, m_Data.bound.max().y, m_Data.bound.max().z );
	
	floats[ m_Id*6 + 0 ] = m_Data.bound.min().x;
	floats[ m_Id*6 + 1 ] = m_Data.bound.min().y;
	floats[ m_Id*6 + 2 ] = m_Data.bound.min().z;
	floats[ m_Id*6 + 3 ] = m_Data.bound.max().x;
	floats[ m_Id*6 + 4 ] = m_Data.bound.max().y;
	floats[ m_Id*6 + 5 ] = m_Data.bound.max().z;
	
	if( ! m_Leaf )
	{
		ints[ m_Id*6 + 0 ] = -1;
		ints[ m_Id*6 + 3 ] = m_Second->id();
		ints[ m_Id*6 + 4 ] = m_FailureLink;
		m_First->write( ints, floats );
		m_Second->write( ints, floats );
	}
	else
	{
		ints[ m_Id*6 + 0 ] = m_Data.type;
		ints[ m_Id*6 + 1 ] = m_Data.index;
		ints[ m_Id*6 + 2 ] = m_Data.geometryIndex;
		ints[ m_Id*6 + 3 ] = m_FailureLink;
		ints[ m_Id*6 + 4 ] = m_FailureLink;
	}
}

}
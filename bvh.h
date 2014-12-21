#include <iostream>
#include <utility>
#include <vector>
#include "common.h"
#include "geometry.h"

namespace BVH
{

struct Primdata
{
	int type;
	int index;
	int shader;
	int shaderIndex;
	AAB_t bound;
};

typedef std::vector< Primdata > Primitives;

class BVHNode
{
public:
	typedef shared_ptr< BVHNode > pointer;
	AAB_t bound(){ return m_Data.bound; }
	virtual void write( int* ints, float* floats ){}
	
protected:
	Primdata m_Data;
};

class BVH
{
public:
	typedef shared_ptr< BVH > pointer;
	BVHNode::pointer root;
	virtual void write( int* ints, float* floats );
	
protected:
	BVH(){};
};

class KP : public BVH
{
public:
	typedef shared_ptr< KP > pointer;
	static pointer build( Primitives );
	int size(){ return m_Size; }
	
private:
	int m_Size;
	KP( Primitives );
};

class KPNode : public BVHNode
{
public:
	enum Axis{ X_Axis, Y_Axis, Z_Axis };
	typedef shared_ptr< KPNode > pointer;
	KPNode( Primitives, Axis axis, int& id, int m_FailureLink );
	int id(){ return m_Id; }
	virtual void write( int* ints, float* floats );
	
private:
	bool m_Leaf;
	int m_Id;
	int m_FailureLink;
	pointer m_First;
	pointer m_Second;
};

}
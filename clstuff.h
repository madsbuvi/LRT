#ifndef clstuff_h
#define clstuff_h

#include <CL/cl.h>
#include <vector>
#include <stdint.h>
#include "clutil.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "common.h"
#include "geometry.h"
#include "bvh.h"

#include <iostream>
using std::ostream;


struct Geometrydata
{
	int primindex;
	int nprim;
	int shader;
	int shaderindex;
};

class DeviceContext
{
	public:
	cl_device_id cldevice;
	cl_context clcontext;
	cl_command_queue clqueue;
	cl_program rtprogram;
	cl_kernel rtkernel;
	
	// Sphere data
	bool spheres_allocated;
	cl_mem sphere_centers_dev;
	cl_mem sphere_radi_dev;
	int n_spheres;
	
	// Triangle data
	bool triangles_allocated;
	cl_mem triangles_dev;
	int n_triangles;
	
	// Box data
	bool boxes_allocated;
	cl_mem boxes_dev;
	cl_mem box_heights_dev;
	int n_boxes;
	
	// AAB data
	bool AAB_allocated;
	cl_mem AAB_dev;
	int n_AABs;
	
	// Geometry data
	bool geometry_allocated;
	cl_mem geometry_dev;
	cl_mem primitives_dev;
	cl_mem shader_data_dev;
	int n_geo;
	
	// Geometry data
	bool bvh_allocated;
	cl_mem bvhfloats_dev;
	cl_mem bvhints_dev;
	int n_bvh;
	
	DeviceContext( unsigned device, GLFWwindow* window );
	void* trace( unsigned width, unsigned height, float3 U, float3 V, float3 W, float3 eye );
	void updateSpheres( std::vector<Sphere>& spheres );
	void updateTriangles( std::vector<Triangle>& triangles );
	void updateAABs( std::vector<AAB_s>& AABs);
	void updateBoxes( std::vector<Box>& boxes );
	void updateGeometry( std::vector<Geometrydata>& gd, std::vector<int>& primitives, std::vector<float>& shader_data );
	void updateBVH( BVH::KP::pointer bvh );
};

class RTContext
{
	std::vector<DeviceContext> devices;
	std::vector<Geometry*> geometry;
	float3 eye;
	float2 angles;
	void updateDevices( void );
	
	int selectedObject;
	bool dirty;
	
	ShaderContext m_ShaderContext;
	
	public:
	RTContext( void );
	void select( int x, int y, unsigned width, unsigned height );
	void deselect( void );
	unsigned registerDeviceContext( DeviceContext dcontext );
	int addGeometry( Geometry* g ){ geometry.push_back( g ); return geometry.size(); };
	int addGeometry( Geometry* g, Shader* shader ){ g->setShader( shader ); geometry.push_back( g ); return geometry.size(); };
	void* trace( unsigned width, unsigned height );
	void step( float mod );
	void strafe( float mod );
	void rmouse( int x, int y, bool ctrl, bool shift, bool alt );
	void lmouse( int x, int y, bool ctrl, bool shift, bool alt );
	ostream& operator<<( ostream& out );
	
	ShaderContext* shaderContext( void ){ return &m_ShaderContext; };
	
};
ostream& operator<<( ostream& out, RTContext& context );

int clinit( void );

#endif
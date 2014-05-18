#ifndef clstuff_h
#define clstuff_h

#include <CL/cl.h>
#include <vector>
#include <stdint.h>
#include "clutil.h"


#ifdef _WIN32
#include <windows.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#endif

#include "gfx_glfw.h"
#include <GLFW/glfw3native.h>
#include "vtypes.h"
#include "geometry.h"
#include "debug.h"


#include <GL/glew.h>
#include <CL/cl_gl.h>
#ifndef _WIN32
#include <GL/glx.h>
#endif




struct Geometrydata
{
	int primindex;
	int nprim;
	int shader;
	int shaderindex;
};

class DeviceContext
{
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
	
	public:
	DeviceContext( unsigned device, GLFWwindow* window );
	void* trace( unsigned width, unsigned height, float3 U, float3 V, float3 W, float3 eye );
	void updateSpheres( std::vector<Sphere_struct>& spheres );
	void updateTriangles( std::vector<Triangle_struct>& triangles );
	void updateAABs( std::vector<AAB_struct>& AABs);
	void updateBoxes( std::vector<Box_struct>& boxes );
	void updateGeometry( std::vector<Geometrydata>& gd, std::vector<int>& primitives, std::vector<float>& shader_data );
};

class RTContext
{
	std::vector<DeviceContext> devices;
	std::vector<Geometry*> geometry;
	float3 eye;
	float2 angles;
	void updateDevices( void );
	public:
	RTContext( void );
	unsigned registerDeviceContext( DeviceContext dcontext );
	int addGeometry( Geometry* g ){ geometry.push_back( g ); return geometry.size(); };
	int addGeometry( Geometry* g, Shader* shader ){ g->setShader( shader ); geometry.push_back( g ); return geometry.size(); };
	void* trace( unsigned width, unsigned height );
	void step( float mod );
	void strafe( float mod );
	void mouse( int x, int y );
	
};

int clinit( void );

#endif
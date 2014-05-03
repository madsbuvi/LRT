#ifndef clstuff_h
#define clstuff_h

#include <CL/cl.h>
#include <vector>
#include <stdint.h>
#include "vtypes.h"
#include "geometry.h"
#include "debug.h"

struct Geometrydata
{
	int primindex;
	int nprim;
	int shaderindex;
	int shadertype;
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
	
	// Triangle data
	bool boxes_allocated;
	cl_mem boxes_dev;
	cl_mem box_heights_dev;
	int n_boxes;
	
	// Geometry data
	bool geometry_allocated;
	cl_mem geometry_dev;
	cl_mem primitives_dev;
	int n_geo;
	
	public:
	DeviceContext( unsigned device );
	void* trace( unsigned width, unsigned height, float3 U, float3 V, float3 W, float3 eye );
	void updateSpheres( std::vector<Sphere_struct>& spheres );
	void updateTriangles( std::vector<Triangle_struct>& triangles );
	void updateBoxes( std::vector<Box_struct>& boxes );
	void updateGeometry( std::vector<Geometrydata>& gd, std::vector<int> primitives );
};

class RTContext
{
	std::vector<DeviceContext> devices;
	std::vector<Geometry> geometry;
	float3 eye;
	float2 angles;
	void updateDevices( void );
	public:
	RTContext( void );
	unsigned registerDeviceContext( DeviceContext dcontext );
	int addGeometry( Geometry g ){ geometry.push_back( g ); return geometry.size(); };
	void* trace( unsigned width, unsigned height );
	void step( float mod );
	void strafe( float mod );
	void mouse( int x, int y );
	
};

int clinit( void );

/*! \brief Performs a binary read of the given file
 *
 * 	Performs a binary read of the given file.
 *	Natural part of cl util because when using opencl you frequently need to read in whole source files
 * 	and keep them in a buffer.
 *
 *	\param[in] filename path to the file to be read.
 *	\param[out]	len	pointer to a size_t variable to store the length of the file read in.
 *	\return	A pointer to a newly allocated buffer with the full contents of the file if successful, NULL otherwise.
 */
const char* readFile(const char *filename, size_t *len);

/*! \brief Get an error string based on error code
 *
 *  Function to get an error string based on error code
 *  Copied from enj @ enja.org
 *	
 *	\param[in] error an OpenCL error code
 *	\return A const string that gives a textual representation of the error.
 */
const char* oclErrorString(cl_int error);

/*! \brief Function to "handle" an error code.
 *
 *  Does nothing other than print out the oclErrorString along with location in source of the error.
 *
 *	\param[in] error an OpenCL error code
 *	\param[in] line Line number where the error occured
 *	\param[in] file Filename of the file where the error occured
 */
void handle_error(cl_int error, int32_t line, const  char *file);

//Defines to handle errors:
//You do not need to use semicolons after these defines, but it's a good habit.

/*!	\brief Define to wrap around a function that returns error code
 *  Define to wrap around a function that returns error code
 *  E.g. HandleErrorRet(clGetPlatformIDs(1, &platform_id, &ret_num_platforms));
 *	Call handle_error on a returned error code and then exits the current thread.
 */
#define HandleErrorRet(clfunc)\
{\
    cl_int error = (clfunc);\
    if(error){\
        handle_error(error, __LINE__, __FILE__);\
    }\
}

//! \brief Alias of the error parameter for HandleErrorPar
#define HANDLE_ERROR &error
/*!	\brief Define to wrap around a function that returns error code to a parameter
 *  Define to wrap around a function that returns error code to a parameter
 *  MAKE SURE YOU PASS "HANDLE_ERROR" AS THE ERROR RETURN PARAMETER
 *	Macro should encompass the entire statement.
 *  E.G. HandleErrorPar(context = clCreateContext(NULL, 1, &device_id, NULL, NULL, HANDLE_ERROR));
 */
#define HandleErrorPar(clfunc)\
{\
    cl_int error;\
    (clfunc);\
    if(error){\
        handle_error(error, __LINE__, __FILE__);\
    }\
}

#endif
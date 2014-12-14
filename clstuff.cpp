#include <cstdlib>
#include <string>
#include <iostream>
#include <stdint.h>
#include "clstuff.h"
#include "oglstuff.h"
#include "texture.h"
#include "shared.h"
#include "host_tracer.h"
static std::vector<cl_device_id> device_vector;




/*! \brief good old malloc for opencl
 *
 *	\param[in] context A context
 *	\param[in] size	How much to allocate
 *	\return A cl_mem object if successful, 0 otherwise.
 */
#define cl_malloc(context, size) cl_malloc_(context, size, __LINE__, __FILE__)
cl_mem cl_malloc_(cl_context context, size_t size, int line, const  char *file){
	cl_mem memory_object;

	cl_int error;
	memory_object = clCreateBuffer(context, CL_MEM_READ_WRITE, size, NULL, &error);
	if(error){
		handle_error(error, line, file);
		return 0;
	}
	return memory_object;
}

int clinit( void )
{
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	uint32_t gpu_devices = 0;
	
	/* Get platforms */
	clGetPlatformIDs(1, NULL, &ret_num_platforms);
	cl_platform_id *platform_ids = new cl_platform_id[ret_num_platforms];
	clGetPlatformIDs(ret_num_platforms, platform_ids, &ret_num_platforms);
	
	/* Count number of GPU devices in each platform */
	unsigned i = 0;
	for(; i < ret_num_platforms; i++){
		clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_GPU, 0, NULL, &ret_num_devices);
		device_vector.resize(device_vector.size() + ret_num_devices);
		clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_GPU, ret_num_devices, &device_vector[gpu_devices], &ret_num_devices);
		gpu_devices += ret_num_devices;
	}
	
	// Discard any and all devices that do not support opengl sharing
	for( i = 0; i < device_vector.size(); i++ )
	{
		char* extensions = new char[10000];
		size_t size;
		clGetDeviceInfo( device_vector[i], CL_DEVICE_EXTENSIONS, 10000, extensions, &size );
		std::string ext(extensions);
		delete extensions;
		if(ext.find("cl_khr_gl_sharing")==std::string::npos)
		{
			device_vector.erase( device_vector.begin()+i );
		}
	}
	
	dprintf( 1, "device_vector size: %d\n", int(device_vector.size()) );
	delete platform_ids;
	
	device_vector.shrink_to_fit();
	
	return device_vector.size();
}

/*
 * Note that right now my entire cl device handling is wonky because i'm only just figuring out opengl interop
 */
DeviceContext::DeviceContext( unsigned device, GLFWwindow* window )
{
	cldevice = device_vector[device];
	
	cl_platform_id pid;
	clGetDeviceInfo( cldevice, CL_DEVICE_PLATFORM, sizeof(pid), &pid, NULL );

	HandleErrorPar( 
		clcontext = clCreateContext( NULL, 1, &cldevice, NULL, NULL, HANDLE_ERROR )
	);
	HandleErrorPar(
		clqueue = clCreateCommandQueue( clcontext, cldevice, CL_QUEUE_PROFILING_ENABLE, HANDLE_ERROR )
	);
	
	size_t len;
	const char* source = readFile("raytracer.cl", &len);
	
	HandleErrorPar(
		rtprogram = clCreateProgramWithSource( clcontext, 1, &source, &len, HANDLE_ERROR )
	);
	
	/* Build Kernel Program */
	cl_int ret = clBuildProgram( rtprogram, 1, &cldevice, "-I ./", NULL, NULL );
	if( 1 ){
		fprintf( stderr, "Error code %d:\n", ret );
		size_t ssiz = 0;
        HandleErrorRet( clGetProgramBuildInfo( rtprogram, cldevice, CL_PROGRAM_BUILD_LOG, 0, NULL, &ssiz ) );
	    char *buildString = (char*)malloc( ssiz+1 );
	    buildString[0]='\0';
        HandleErrorRet( clGetProgramBuildInfo( rtprogram, cldevice, CL_PROGRAM_BUILD_LOG, ssiz, buildString, NULL ) );
        printf( "build log:\n%s\n", buildString );
		free( buildString );
	}
	
	HandleErrorPar(
		rtkernel = clCreateKernel( rtprogram, "raytrace", HANDLE_ERROR )
	);
	
	spheres_allocated = false;
	triangles_allocated = false;
	boxes_allocated = false;
	geometry_allocated = false;
	AAB_allocated = false;
}


void* DeviceContext::trace( unsigned width, unsigned height, float3 U, float3 V, float3 W, float3 eye )
{
	uint32_t* output;
	cl_mem devmem = cl_malloc( clcontext, width*height*sizeof(*output) );

	
	float3 light = { -200.f, 60.f, -200.f };
	
	// This stuff is just a temporary way of ordering stuff.
	static bool texloaded = false;
	static cl_mem pack;
	if(!texloaded)
	{
		pack = Texture::compileTextureImage( clcontext, clqueue );
		texloaded = true;
	}
	
	// Why can't i hold all these kernel arguments
	int arg = 0;
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(U), &U) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(V), &V) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(W), &W) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(eye), &eye) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(cl_mem), &devmem) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(cl_mem), &sphere_centers_dev) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(cl_mem), &sphere_radi_dev) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(int), &n_spheres) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(cl_mem), &triangles_dev) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(int), &n_triangles) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(cl_mem), &AAB_dev) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(int), &n_AABs) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(cl_mem), &boxes_dev) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(cl_mem), &box_heights_dev) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(int), &n_boxes) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(cl_mem), &geometry_dev) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(cl_mem), &primitives_dev) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(int), &n_geo) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(light), &light) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(cl_mem), &shader_data_dev) );
	HandleErrorRet( clSetKernelArg( rtkernel, arg++, sizeof(cl_mem), &pack) );
	size_t global[] = {width, height};
	size_t local[] = {16, 16};
	
	HandleErrorRet(
		clEnqueueNDRangeKernel( clqueue, rtkernel, 2, NULL, global, local, 0, NULL, NULL )
	)
	
	output = (uint32_t*)calloc( width * height, sizeof(*output) );	
	
	HandleErrorRet(
		clEnqueueReadBuffer( clqueue, devmem, true, 0, width*height*sizeof(*output), (void*)output, 0, NULL, NULL )
	);
	
    HandleErrorRet(
		clReleaseMemObject( devmem )
	);
	return (void*) output;
}

/*
 *	Calculate pinhole camera vectors.
 *	eye  - the position of the camera
 *	lookat - what the camera is looking at (view direction of the center of our view will be towards this, with focus length equal to | lookat - eye |
 *	up - The direction up from the eye. Should be orthogonal to ( lookat - eye )
 *  Hfov - an angle defining the horizontal field of view. The vertical field of view becomes hfov/aspect_ratio
 *  Aspect_ratio - width/height (resolution)
 *  W - Vector from eye to lookat
 *  U - Vector, orthogonal to both up and W, defining the "vertical" sides of the picture ( lookat + U = lefthand side )
 *	V - Vector, orthogonal to U and W, and parallel to up ( if up is orthogonal to W ), defining the top/botton of the picture ( lookat + V = the bottom of the screen )
 *
 *	If the picture buffer you want to fill does not start with ( 0, 0 ) in the bottom left corner of the picture, take the negative(s) of U and/or V.
 *	In this project the buffer starts with ( 0, 0 ) at the top left corner of the picture, therefore I use the negative of V.
 *	To creature the lookat argument, I pass ( eye + direction * focus_length ). Where direction is created by spheric coordinates.
 *	To create the up argument, i once again just use spheric coordinates, but subtract M_PI/2 from the relevant angle.
 */
static void calculateCameraVariables( float3 eye,
									float3 lookat,
									float3 up,
									float hfov,
									float aspect_ratio,
									float3& U,
									float3& V,
									float3& W )
{
	float ulen, vlen, wlen;
	W = lookat - eye;
	
	wlen = sqrtf( dot( W, W ) );
	
	U = normalize( cross( W, up ) );
	V = normalize( cross( U, W ) );
	
	ulen = wlen * tanf( hfov / 2.0f * 3.14159265358979323846f / 180.0f );
	U = U * ulen;
	vlen =  ulen/aspect_ratio;
	V = V * vlen;
}

// Translates spherical coordinates to cartesian, using a radius of 1
static float3 spheric(float2 angles)
{
	float3 out;
	out.x = sinf(angles.y)*cosf(angles.x);
	out.y = cosf(angles.y);
	out.z = sinf(angles.y)*sinf(angles.x);
	return out;
}

void DeviceContext::updateSpheres( std::vector<Sphere>& spheres )
{
	if( spheres_allocated )
	{
		HandleErrorRet(
			clReleaseMemObject( sphere_centers_dev )
		);
		HandleErrorRet(
			clReleaseMemObject( sphere_radi_dev )
		);
	}
	
	n_spheres = spheres.size();
	
	if( n_spheres )
	{
		sphere_centers_dev = cl_malloc( clcontext, n_spheres * sizeof( float3 ) );
		sphere_radi_dev = cl_malloc( clcontext, n_spheres * sizeof( float ) );
		spheres_allocated = true;
		float3* centers;
		float*  radi;
		HandleErrorPar(
			centers = (float3*)clEnqueueMapBuffer( clqueue, sphere_centers_dev, true, CL_MAP_WRITE, 0, n_spheres * sizeof( float3 ), 0, NULL, NULL, HANDLE_ERROR )
		);
		HandleErrorPar(
			radi = (float*)clEnqueueMapBuffer( clqueue, sphere_radi_dev, true, CL_MAP_WRITE, 0, n_spheres * sizeof( float ), 0, NULL, NULL, HANDLE_ERROR )
		);
		
		for( int i = 0; i < n_spheres; i++ )
		{
			centers[i] = spheres[i].center;
			radi[i] = spheres[i].radius;
		}
		HandleErrorRet(
			clEnqueueUnmapMemObject( clqueue, sphere_centers_dev, (void*)centers, 0, NULL, NULL )
		);
		HandleErrorRet(
			clEnqueueUnmapMemObject( clqueue, sphere_radi_dev, (void*)centers, 0, NULL, NULL )
		);
		
	}
	else
	{
		// Allocate a dummy object, as the AMD implementation will segfault if i try to launch with an unallocated buffer
		sphere_centers_dev = cl_malloc( clcontext, 128 );
		sphere_radi_dev = cl_malloc( clcontext, 128 );
		spheres_allocated = true;
	}
}


void DeviceContext::updateTriangles( std::vector<Triangle>& triangles )
{
	if( triangles_allocated )
	{
		HandleErrorRet(
			clReleaseMemObject( triangles_dev )
		);
	}
	
	n_triangles = triangles.size();
	
	if( n_triangles )
	{
		triangles_dev = cl_malloc( clcontext, n_triangles * 3 * sizeof( float3 ) );
		triangles_allocated = true;
		float3* array;
		HandleErrorPar(
			array = (float3*)clEnqueueMapBuffer( clqueue, triangles_dev, true, CL_MAP_WRITE, 0, n_triangles * 3 * sizeof( float3 ), 0, NULL, NULL, HANDLE_ERROR )
		);
		
		for( int i = 0; i < n_triangles; i++ )
		{
			array[ i*3 ] = triangles[i].v1;
			array[ i*3+1 ] = triangles[i].v2;
			array[ i*3+2 ] = triangles[i].v3;
		}
		HandleErrorRet(
			clEnqueueUnmapMemObject( clqueue, triangles_dev, (void*)array, 0, NULL, NULL )
		);
		
	}
	else
	{
		// Allocate a dummy object, as the AMD implementation will segfault if i try to launch with an unallocated buffer
		triangles_dev = cl_malloc( clcontext, 128 );
		triangles_allocated = true;
	}
}

void DeviceContext::updateAABs( std::vector<AAB_s>& AABs)
{
	if( AAB_allocated )
	{
		HandleErrorRet(
			clReleaseMemObject( AAB_dev )
		);
	}
	
	n_AABs = AABs.size();
	
	if( n_AABs )
	{
		AAB_dev = cl_malloc( clcontext, n_AABs * 2 * sizeof( float3 ) );
		AAB_allocated = true;
		float3* array;
		HandleErrorPar(
			array = (float3*)clEnqueueMapBuffer( clqueue, AAB_dev, true, CL_MAP_WRITE, 0, n_AABs * 2 * sizeof( float3 ), 0, NULL, NULL, HANDLE_ERROR )
		);
		
		for( int i = 0; i < n_AABs; i++ )
		{
			array[ i*2 ] = AABs[i].bmin;
			array[ i*2+1 ] = AABs[i].bmax;
		}
		HandleErrorRet(
			clEnqueueUnmapMemObject( clqueue, AAB_dev, (void*)array, 0, NULL, NULL )
		);
		
	}
	else
	{
		// Allocate a dummy object, as the AMD implementation will segfault if i try to launch with an unallocated buffer
		AAB_dev = cl_malloc( clcontext, 128 );
		AAB_allocated = true;
	}
}

void DeviceContext::updateBoxes( std::vector<Box>& boxes)
{
	if( boxes_allocated )
	{
		HandleErrorRet(
			clReleaseMemObject( boxes_dev )
		);
		HandleErrorRet(
			clReleaseMemObject( box_heights_dev )
		);
	}
	
	n_boxes = boxes.size();
	
	if( n_boxes )
	{
		boxes_dev = cl_malloc( clcontext, n_boxes * 3 * sizeof( float3 ) );
		box_heights_dev = cl_malloc( clcontext, n_boxes * sizeof( float ) );
		boxes_allocated = true;
		float3* array;
		float* heights;
		HandleErrorPar(
			array = (float3*)clEnqueueMapBuffer( clqueue, boxes_dev, true, CL_MAP_WRITE, 0, n_boxes * 3 * sizeof( float3 ), 0, NULL, NULL, HANDLE_ERROR )
		);
		HandleErrorPar(
			heights = (float*)clEnqueueMapBuffer( clqueue, box_heights_dev, true, CL_MAP_WRITE, 0, n_boxes * sizeof( float ), 0, NULL, NULL, HANDLE_ERROR )
		);
		
		for( int i = 0; i < n_boxes; i++ )
		{
			array[ i*3 ] = boxes[i].s1;
			array[ i*3+1 ] = boxes[i].s2;
			array[ i*3+2 ] = boxes[i].s3;
			heights[ i ] = boxes[i].h;
		}
		HandleErrorRet(
			clEnqueueUnmapMemObject( clqueue, boxes_dev, (void*)array, 0, NULL, NULL )
		);
		HandleErrorRet(
			clEnqueueUnmapMemObject( clqueue, box_heights_dev, (void*)heights, 0, NULL, NULL )
		);
		
	}
	else
	{
		// Allocate a dummy object, as the AMD implementation will segfault if i try to launch with an unallocated buffer
		boxes_dev = cl_malloc( clcontext, 128 );
		box_heights_dev = cl_malloc( clcontext, 128 );
		boxes_allocated = true;
	}
}

void DeviceContext::updateGeometry( std::vector<Geometrydata>& gd, std::vector<int>& primitives, std::vector<float>& shader_data )
{
	if( geometry_allocated )
	{
		HandleErrorRet(
			clReleaseMemObject( geometry_dev )
		);
		HandleErrorRet(
			clReleaseMemObject( primitives_dev )
		);
		HandleErrorRet(
			clReleaseMemObject( shader_data_dev )
		);
	}
	
	n_geo = gd.size();
	int n_prim = primitives.size();
	int n_shader_data = shader_data.size();
	if( n_geo && n_prim )
	{
		geometry_dev = cl_malloc( clcontext, n_geo * 4 * sizeof( int ) );
		primitives_dev = cl_malloc( clcontext, n_prim * sizeof( int ) );
		shader_data_dev = cl_malloc( clcontext, n_shader_data * sizeof( float ) );
		geometry_allocated = true;
		int* geo;
		int* prim;
		float* sd;
		HandleErrorPar(
			geo= (int*)clEnqueueMapBuffer( clqueue, geometry_dev, true, CL_MAP_WRITE, 0, n_geo * 4 * sizeof( int ), 0, NULL, NULL, HANDLE_ERROR )
		);
		HandleErrorPar(
			prim = (int*)clEnqueueMapBuffer( clqueue, primitives_dev, true, CL_MAP_WRITE, 0, n_prim * sizeof( int ), 0, NULL, NULL, HANDLE_ERROR )
		);
		HandleErrorPar(
			sd = (float*)clEnqueueMapBuffer( clqueue, shader_data_dev, true, CL_MAP_WRITE, 0, n_shader_data * sizeof( float ), 0, NULL, NULL, HANDLE_ERROR )
		);

		for( int i = 0; i < n_geo; i++ )
		{
			geo[ i*4 ] = gd[i].primindex;
			geo[ i*4 + 1 ] = gd[i].nprim;
			geo[ i*4 + 2 ] = gd[i].shader;
			geo[ i*4 + 3 ] = gd[i].shaderindex;
		}
		
		for( int i = 0; i < n_prim; i++ )
		{
			prim[ i ] = primitives[ i ];
		}
		
		for( int i = 0; i < n_shader_data; i++ )
		{
			sd[ i ] = shader_data[ i ];
		}
		
		
		
		HandleErrorRet(
			clEnqueueUnmapMemObject( clqueue, geometry_dev, (void*)geo, 0, NULL, NULL )
		);
		HandleErrorRet(
			clEnqueueUnmapMemObject( clqueue, primitives_dev, (void*)prim, 0, NULL, NULL )
		);
		HandleErrorRet(
			clEnqueueUnmapMemObject( clqueue, shader_data_dev, (void*)sd, 0, NULL, NULL )
		);
		
	}
	else
	{
		// Allocate a dummy object, as the AMD implementation will segfault if i try to launch with an unallocated buffer
		geometry_dev = cl_malloc( clcontext, 128 );
		primitives_dev = cl_malloc( clcontext, 128 );
		shader_data_dev = cl_malloc( clcontext, 128 );
		geometry_allocated = true;
	}
}

void RTContext::updateDevices( void )
{
	std::vector<Sphere> spheres;
	std::vector<Triangle> triangles;
	std::vector<Quadrilateral> quads;
	std::vector<AAB_s> AABs;
	std::vector<Box> boxes;
	std::vector<Geometrydata> gd;
	std::vector<int> primitives;
	std::vector<float> shader_data;
	
	for( Geometry* geo: geometry )
	{
		Geometrydata data;
		data.primindex = primitives.size();
		data.nprim = geo->getPrimitives()->size();
		data.shader = geo->getShader()->shader;
		data.shaderindex = shader_data.size();
		geo->getShader()->writeShaderData(shader_data);
		
		for( Primitive* p: *geo->getPrimitives() )
		{
			switch( p->getType() )
			{
				case SPHERE:
					spheres.push_back( static_cast<Sphere_t*>(p)->s );
					primitives.push_back( 0 );
					primitives.push_back( spheres.size() - 1 );
					break;
				case TRIANGLE:
					triangles.push_back( static_cast<Triangle_t*>(p)->s );
					primitives.push_back( 1 );
					primitives.push_back( triangles.size() - 1 );
					break;
				case QUADRILATERAL:
					quads.push_back( static_cast<Quadrilateral_t*>(p)->s );
					primitives.push_back( 3 );
					primitives.push_back( quads.size() - 1 );
					break;
				case AAB:
					AABs.push_back( static_cast<AAB_t*>(p)->s );
					primitives.push_back( 4 );
					primitives.push_back( AABs.size() - 1 );
					break;
				case BOX:
					boxes.push_back( static_cast<Box_t*>(p)->s );
					primitives.push_back( 5 );
					primitives.push_back( boxes.size() - 1 );
					break;
			}
		}
		
		gd.push_back( data );
	}
	
	
	devices[0].updateSpheres( spheres );
	devices[0].updateTriangles( triangles );
	devices[0].updateBoxes( boxes );
	devices[0].updateGeometry( gd, primitives, shader_data );
	devices[0].updateAABs( AABs );
}

void* RTContext::trace( unsigned width, unsigned height )
{
	float3 a2, a3;
	
	a2 = eye + spheric(angles);
	a3 = spheric( make_float2( angles.x, angles.y - M_PI/2.f ) );
	
	float3 U, V, W;
	
	
	calculateCameraVariables( eye, a2, a3, 45,
				float(width) / float(height),
				U, V, W);
	
	
	if(devices.size() && dirty){
		updateDevices();
		return devices[0].trace( width, height, U, -V, W, eye );
		dirty = false;
	}
	dprintf( 1, "Attempted to trace without a device to trace on!\n" );
	return NULL;
}

void RTContext::select( int x, int y, unsigned width, unsigned height )
{
	float3 a2, a3;
	
	if(x < 0 || y < 0)
	{
		dprintf( 2, "Attempted to select negative coordinates: %d, %d\n", x, y );
		return;
	}
	
	a2 = eye + spheric(angles);
	a3 = spheric( make_float2( angles.x, angles.y - M_PI/2.f ) );
	
	float3 U, V, W;
	
	
	calculateCameraVariables( eye, a2, a3, 45,
				float(width) / float(height),
				U, V, W);

	float2 d1 = {float(x), float(y)};
	float2 d2 = {float(width), float(height)};
	float2 d = d1 / d2 * 2.f - 1.f;
	float3 ray_origin = eye;
	float3 ray_direction = normalize(d.x*U + d.y*V + W);
	Ray ray = make_ray(ray_origin, ray_direction, 0, RT_DEFAULT_MIN, RT_DEFAULT_MAX);
	selectedObject = selectTrace(ray, geometry);
	if(selectedObject >= 0){
		assert( selectedObject < int(geometry.size()) );
		geometry[ selectedObject ] -> select( );
	}
}

void RTContext::deselect( void )
{
	if( selectedObject >= 0 )geometry[ selectedObject ] -> deselect( );
	selectedObject = -1;
}

void RTContext::step( float mod )
{
	eye = eye + mod*spheric( angles );
}

void RTContext::strafe( float mod )
{	
	float3 direction = spheric( make_float2(angles.x - M_PI/2.f, angles.y) );
	direction.y = 0.f;
	direction = mod*normalize(direction);
	eye = eye + direction;
}

void RTContext::rmouse( int x, int y, bool ctrl, bool shift, bool alt )
{
	angles.x += 0.01f * float(x);
	angles.y -= 0.01f * float(y);
}

void RTContext::lmouse( int x, int y, bool ctrl, bool shift, bool alt )
{
	if(selectedObject>=0)
	{
		float3 d;

		if(alt)
		{
			d = make_float3( 0.f, float(x) + float(y), 0.f );
			
		}
		else
		{
			float3 direction;
			float dx, dz;
			dx = -float(x);
			dz = float(y);
			direction = spheric( make_float2(angles.x - M_PI/2.f, angles.y) );
			direction.y = 0.f;
			d = dx*normalize(direction);
			direction = spheric( make_float2(angles.x, angles.y) );
			direction.y = 0.f;
			d+= dz*normalize(direction);
		}
	
		geometry[ selectedObject ] -> move( d / 100.f );
	}
}

RTContext::RTContext( void )
{
	eye = make_float3( 2, 1, 0 );
	angles = make_float2( 1.52f, 1.81f );
	selectedObject = -1;
	dirty = true;
}

unsigned RTContext::registerDeviceContext( DeviceContext dcontext )
{
	devices.push_back( dcontext );
	return devices.size();
}

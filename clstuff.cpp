#include <cstdlib>
#include <stdint.h>
#include "clstuff.h"

static std::vector<cl_device_id> device_vector;

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

/*! \brief good old malloc for opencl
 *
 *	\param[in] context A context
 *	\param[in] size	How much to allocate
 *	\return A cl_mem object if successful, 0 otherwise.
 */
#define cl_malloc(context, size) cl_malloc_(context, size, __LINE__, __FILE__)

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
	
	dprintf( 1, "device_vector size: %d\n", device_vector.size() );
	delete platform_ids;
	return gpu_devices;
}

DeviceContext::DeviceContext( unsigned device )
{
	cldevice = device_vector[device];
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
	cl_int ret = clBuildProgram( rtprogram, 1, &cldevice, "", NULL, NULL );
	if( ret ){
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
}


void* DeviceContext::trace( unsigned width, unsigned height, float3 U, float3 V, float3 W, float3 eye )
{
	uint32_t* output;
	cl_mem devmem = cl_malloc( clcontext, width*height*sizeof(*output) );

	
	HandleErrorRet( clSetKernelArg( rtkernel, 0, sizeof(U), &U) );
	HandleErrorRet( clSetKernelArg( rtkernel, 1, sizeof(V), &V) );
	HandleErrorRet( clSetKernelArg( rtkernel, 2, sizeof(W), &W) );
	HandleErrorRet( clSetKernelArg( rtkernel, 3, sizeof(eye), &eye) );
	HandleErrorRet( clSetKernelArg( rtkernel, 4, sizeof(cl_mem), &devmem) );
	HandleErrorRet( clSetKernelArg( rtkernel, 5, sizeof(cl_mem), &sphere_centers_dev) );
	HandleErrorRet( clSetKernelArg( rtkernel, 6, sizeof(cl_mem), &sphere_radi_dev) );
	HandleErrorRet( clSetKernelArg( rtkernel, 7, sizeof(int), &n_spheres) );
	HandleErrorRet( clSetKernelArg( rtkernel, 8, sizeof(cl_mem), &triangles_dev) );
	HandleErrorRet( clSetKernelArg( rtkernel, 9, sizeof(int), &n_triangles) );
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

RTContext::RTContext( void )
{
	eye = make_float3( 0.f, 0.f, 0.f );
	angles = make_float2( 0.f, 1.57f );
}

unsigned RTContext::registerDeviceContext( DeviceContext dcontext )
{
	devices.push_back( dcontext );
	return devices.size();
}

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

void DeviceContext::updateSpheres( std::vector<Sphere_struct>& spheres )
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


void DeviceContext::updateTriangles( std::vector<Triangle_struct>& triangles )
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

void RTContext::updateDevices( void )
{
	std::vector<Sphere_struct> spheres;
	std::vector<Triangle_struct> triangles;
	std::vector<Rectangle_struct> rectangles;
	std::vector<Quadrilateral_struct> quads;
	std::vector<AAB_struct> AABs;
	std::vector<Box_struct> boxes;
	
	for( Geometry geo: geometry )
	{
		for( Primitive* p: *geo.getPrimitives() )
		{
			switch( p->getType() )
			{
				case Sphere_t:
					spheres.push_back( static_cast<Sphere*>(p)->s );
					break;
				case Triangle_t:
					triangles.push_back( static_cast<Triangle*>(p)->s );
					break;
				case Rectangle_t:
					rectangles.push_back( static_cast<Rectangle*>(p)->s );
					break;
				case Quadrilateral_t:
					quads.push_back( static_cast<Quadrilateral*>(p)->s );
					break;
				case AAB_t:
					AABs.push_back( static_cast<AAB*>(p)->s );
					break;
				case Box_t:
					boxes.push_back( static_cast<Box*>(p)->s );
					break;
			}
		}
	}
	
	devices[0].updateSpheres( spheres );
	devices[0].updateTriangles( triangles );
}

void* RTContext::trace( unsigned width, unsigned height )
{
	float3 a2, a3;
	
	a2 = eye + spheric(angles);
	a3 = spheric( make_float2( angles.x, angles.y - M_PI/2.f ) );
	
	float3 U, V, W;
	
	
	calculateCameraVariables( eye, a2, a3, 45,
				static_cast<float>(width) / static_cast<float>(height),
				U, V, W);
	
	
	if(devices.size()){
		updateDevices();
		return devices[0].trace( width, height, U, V, W, eye );
	}
	dprintf( 1, "Attempted to trace without a device to trace on!\n" );
	return NULL;
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

void RTContext::mouse( int x, int y )
{
	angles.x += 0.01f * static_cast<float>(x);
	angles.y -= 0.01f * static_cast<float>(y);
}

const char* readFile(const char *filename, size_t *len)
{

	FILE* file;
	uint8_t* buffer;
	uint64_t fileLen;

	//Open file
	file = fopen(filename, "rb");
	if (!file)
	{
		fprintf(stderr, "Unable to open file %s", filename);
		return NULL;
	}

	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
	*len = fileLen;
	fseek(file, 0, SEEK_SET);

	//Allocate memory
	buffer=(uint8_t *)calloc(1,fileLen+1);
	if (!buffer)
	{
		fprintf(stderr, "Memory error!");
        fclose(file);
		return NULL;
	}
	//Read file contents into buffer
	fread(buffer, fileLen, 1, file);
	buffer[fileLen]='\0';
	fclose(file);
	return (const char *)buffer;
}

const char* oclErrorString(cl_int error)
{
    static const char* errorString[] = {
        "CL_SUCCESS",
        "CL_DEVICE_NOT_FOUND",
        "CL_DEVICE_NOT_AVAILABLE",
        "CL_COMPILER_NOT_AVAILABLE",
        "CL_MEM_OBJECT_ALLOCATION_FAILURE",
        "CL_OUT_OF_RESOURCES",
        "CL_OUT_OF_HOST_MEMORY",
        "CL_PROFILING_INFO_NOT_AVAILABLE",
        "CL_MEM_COPY_OVERLAP",
        "CL_IMAGE_FORMAT_MISMATCH",
        "CL_IMAGE_FORMAT_NOT_SUPPORTED",
        "CL_BUILD_PROGRAM_FAILURE",
        "CL_MAP_FAILURE",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "CL_INVALID_VALUE",
        "CL_INVALID_DEVICE_TYPE",
        "CL_INVALID_PLATFORM",
        "CL_INVALID_DEVICE",
        "CL_INVALID_CONTEXT",
        "CL_INVALID_QUEUE_PROPERTIES",
        "CL_INVALID_COMMAND_QUEUE",
        "CL_INVALID_HOST_PTR",
        "CL_INVALID_MEM_OBJECT",
        "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
        "CL_INVALID_IMAGE_SIZE",
        "CL_INVALID_SAMPLER",
        "CL_INVALID_BINARY",
        "CL_INVALID_BUILD_OPTIONS",
        "CL_INVALID_PROGRAM",
        "CL_INVALID_PROGRAM_EXECUTABLE",
        "CL_INVALID_KERNEL_NAME",
        "CL_INVALID_KERNEL_DEFINITION",
        "CL_INVALID_KERNEL",
        "CL_INVALID_ARG_INDEX",
        "CL_INVALID_ARG_VALUE",
        "CL_INVALID_ARG_SIZE",
        "CL_INVALID_KERNEL_ARGS",
        "CL_INVALID_WORK_DIMENSION",
        "CL_INVALID_WORK_GROUP_SIZE",
        "CL_INVALID_WORK_ITEM_SIZE",
        "CL_INVALID_GLOBAL_OFFSET",
        "CL_INVALID_EVENT_WAIT_LIST",
        "CL_INVALID_EVENT",
        "CL_INVALID_OPERATION",
        "CL_INVALID_GL_OBJECT",
        "CL_INVALID_BUFFER_SIZE",
        "CL_INVALID_MIP_LEVEL",
        "CL_INVALID_GLOBAL_WORK_SIZE",
    };

    const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

    const int index = -error;

    return (index >= 0 && index < errorCount) ? errorString[index] : "";

}

void handle_error(cl_int error, int32_t line, const  char *file){
    fprintf( stderr,"Error encountered, code %d:\nAt line: %d\nIn file: %s\nError text:%s\n",
            error, line, file, oclErrorString(error) );
	exit(0);
}
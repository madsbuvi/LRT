#include <cstdlib>
#include <stdint.h>
#include "clstuff.h"

static std::vector<cl_device_id> device_vector;

int clinit( void )
{
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	uint32_t gpu_devices = 0;
	
	/* Get platforms */
	clGetPlatformIDs(1, NULL, &ret_num_platforms);
	cl_platform_id platform_ids[ret_num_platforms];
	clGetPlatformIDs(ret_num_platforms, platform_ids, &ret_num_platforms);
	
	/* Count number of GPU devices in each platform */
	int i = 0;
	for(; i < ret_num_platforms; i++){
		clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_GPU, 0, NULL, &ret_num_devices);
		device_vector.resize(device_vector.size() + ret_num_devices);
		clGetDeviceIDs(platform_ids[i], CL_DEVICE_TYPE_GPU, ret_num_devices, &device_vector[gpu_devices], &ret_num_devices);
		gpu_devices += ret_num_devices;
	}
	
	dprintf( 1, "device_vector size: %d\n", device_vector.size() );
	
	return gpu_devices;
}

device_context create_clcontext( unsigned device )
{
	device_context context;
	context.cldevice = device_vector[device];
	HandleErrorPar( 
		context.clcontext = clCreateContext( NULL, 1, &device_vector[device], NULL, NULL, HANDLE_ERROR )
	);
	HandleErrorPar(
		context.clqueue = clCreateCommandQueue( context.clcontext, context.cldevice, CL_QUEUE_PROFILING_ENABLE, HANDLE_ERROR )
	);
	
	size_t len;
	const char* source = readFile("raytracer.cl", &len);
	
	HandleErrorPar(
		context.rtprogram = clCreateProgramWithSource( context.clcontext, 1, &source, &len, HANDLE_ERROR )
	);
	
	/* Build Kernel Program */
	cl_int ret = clBuildProgram(context.rtprogram, 1, &context.cldevice, "", NULL, NULL);
	if(ret){
		fprintf(stderr, "Error code %d:\n", ret);
		size_t ssiz = 0;
        HandleErrorRet(clGetProgramBuildInfo(context.rtprogram, context.cldevice, CL_PROGRAM_BUILD_LOG, 0, NULL, &ssiz));
	    char *buildString = (char*)malloc(ssiz+1);
	    buildString[0]='\0';
        HandleErrorRet(clGetProgramBuildInfo(context.rtprogram, context.cldevice, CL_PROGRAM_BUILD_LOG, ssiz, buildString, NULL));
        printf("build log:\n%s\n", buildString);
		free(buildString);
	}
	
	
	return context;
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
}
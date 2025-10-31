#define CL_TARGET_OPENCL_VERSION 200

#include <stdio.h>
#include "CL/opencl.h"
#include <stdlib.h>
#include <chrono>
#include <string.h>
#include <iostream>
#include <math.h>

#include "timestamp.h"

#include <malloc.h>


using namespace std;


#include "opencl_kernels.h"
#define KERNEL_STRING opencl_kernel_code


const char *getErrorString(cl_int error)
{
	switch (error){
		// run-time and JIT compiler errors
	case 0: return "CL_SUCCESS";
	case -1: return "CL_DEVICE_NOT_FOUND";
	case -2: return "CL_DEVICE_NOT_AVAILABLE";
	case -3: return "CL_COMPILER_NOT_AVAILABLE";
	case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case -5: return "CL_OUT_OF_RESOURCES";
	case -6: return "CL_OUT_OF_HOST_MEMORY";
	case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case -8: return "CL_MEM_COPY_OVERLAP";
	case -9: return "CL_IMAGE_FORMAT_MISMATCH";
	case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case -11: return "CL_BUILD_PROGRAM_FAILURE";
	case -12: return "CL_MAP_FAILURE";
	case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case -15: return "CL_COMPILE_PROGRAM_FAILURE";
	case -16: return "CL_LINKER_NOT_AVAILABLE";
	case -17: return "CL_LINK_PROGRAM_FAILURE";
	case -18: return "CL_DEVICE_PARTITION_FAILED";
	case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

		// compile-time errors
	case -30: return "CL_INVALID_VALUE";
	case -31: return "CL_INVALID_DEVICE_TYPE";
	case -32: return "CL_INVALID_PLATFORM";
	case -33: return "CL_INVALID_DEVICE";
	case -34: return "CL_INVALID_CONTEXT";
	case -35: return "CL_INVALID_QUEUE_PROPERTIES";
	case -36: return "CL_INVALID_COMMAND_QUEUE";
	case -37: return "CL_INVALID_HOST_PTR";
	case -38: return "CL_INVALID_MEM_OBJECT";
	case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case -40: return "CL_INVALID_IMAGE_SIZE";
	case -41: return "CL_INVALID_SAMPLER";
	case -42: return "CL_INVALID_BINARY";
	case -43: return "CL_INVALID_BUILD_OPTIONS";
	case -44: return "CL_INVALID_PROGRAM";
	case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
	case -46: return "CL_INVALID_KERNEL_NAME";
	case -47: return "CL_INVALID_KERNEL_DEFINITION";
	case -48: return "CL_INVALID_KERNEL";
	case -49: return "CL_INVALID_ARG_INDEX";
	case -50: return "CL_INVALID_ARG_VALUE";
	case -51: return "CL_INVALID_ARG_SIZE";
	case -52: return "CL_INVALID_KERNEL_ARGS";
	case -53: return "CL_INVALID_WORK_DIMENSION";
	case -54: return "CL_INVALID_WORK_GROUP_SIZE";
	case -55: return "CL_INVALID_WORK_ITEM_SIZE";
	case -56: return "CL_INVALID_GLOBAL_OFFSET";
	case -57: return "CL_INVALID_EVENT_WAIT_LIST";
	case -58: return "CL_INVALID_EVENT";
	case -59: return "CL_INVALID_OPERATION";
	case -60: return "CL_INVALID_GL_OBJECT";
	case -61: return "CL_INVALID_BUFFER_SIZE";
	case -62: return "CL_INVALID_MIP_LEVEL";
	case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
	case -64: return "CL_INVALID_PROPERTY";
	case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
	case -66: return "CL_INVALID_COMPILER_OPTIONS";
	case -67: return "CL_INVALID_LINKER_OPTIONS";
	case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

		// extension errors
	case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
	case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
	case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
	case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
	case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
	case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
	default: return "Unknown OpenCL error";
	}
}

struct ocl_data_t {
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_bool shared_mem;
	cl_program program = NULL;
	cl_kernel kernel = NULL;

	int buff_size_coeff;
	cl_mem coeff_host_clmem;
	cl_mem coeff_device_clmem;
	void *coeff_host;

	int buff_size_src;
	cl_mem src_host_clmem;
	cl_mem src_device_clmem;
	void *src_host;

	int buff_size_dst;
	cl_mem dst_host_clmem;
	cl_mem dst_device_clmem;
	void *dst_host;

} ocl_data;


int ocl_init(int ocl_dev)
{
	cl_int ret;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;

	/* Get Platform and Device Info */
	ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
	cl_platform_id *platforms;
	platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id)* ret_num_platforms);
	ret = clGetPlatformIDs(ret_num_platforms, platforms, &ret_num_platforms);

	int num_devices_all = 0;
	for (cl_uint platform_id = 0; platform_id < ret_num_platforms; platform_id++) {
		ret = clGetDeviceIDs(platforms[platform_id], CL_DEVICE_TYPE_ALL, 0, NULL, &ret_num_devices);
		num_devices_all = num_devices_all + ret_num_devices;
	}

	if (num_devices_all<1) {
		printf("OpenCL error: no device found\n");
		return -1;
	}

	cl_device_id *devices;
	int device_offset = 0;
	devices = (cl_device_id*)malloc(sizeof(cl_device_id)* num_devices_all);
	for (cl_uint platform_id = 0; platform_id < ret_num_platforms; platform_id++) {
		ret = clGetDeviceIDs(platforms[platform_id], CL_DEVICE_TYPE_ALL, 0, NULL, &ret_num_devices);
		ret = clGetDeviceIDs(platforms[platform_id], CL_DEVICE_TYPE_ALL, ret_num_devices, &devices[device_offset], &ret_num_devices);
		device_offset = device_offset + ret_num_devices;
	}

	char cBuffer[1024];
	for (int device_num = 0; device_num < num_devices_all; device_num++) {
		printf("Device id: %d,  ", device_num);

		ret = clGetDeviceInfo(devices[device_num], CL_DEVICE_VENDOR, sizeof(cBuffer), &cBuffer, NULL);
		printf("%s ", cBuffer);

		ret = clGetDeviceInfo(devices[device_num], CL_DEVICE_NAME, sizeof(cBuffer), &cBuffer, NULL);
		printf("%s\r\n", cBuffer);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Select device to be used */
	if (ocl_dev==-1) {
		printf("Select OpenCL device and press enter:");
		ocl_dev = getchar()-0x30;
	}
	if (ocl_dev<0 || ocl_dev>num_devices_all) {
		ocl_dev = 0;
	}
	ocl_data.device_id = devices[ocl_dev];
	free(devices);
	printf("OpenCL selected device: %d\n", ocl_dev);

	/* Create OpenCL context */
	ocl_data.context = clCreateContext(NULL, 1, &ocl_data.device_id, NULL, NULL, &ret);
	if (ret != CL_SUCCESS) {
		printf("OpenCL error clCreateContext %s\n", getErrorString(ret));
		return -1;
	}


	/* Check if device has shared memory */
	cl_device_info param_name = CL_DEVICE_HOST_UNIFIED_MEMORY;
	ret = clGetDeviceInfo(ocl_data.device_id, param_name, sizeof(ocl_data.shared_mem), (void*)(&ocl_data.shared_mem), NULL);
	printf("OpenCL device Global Memory type: %s\n", ocl_data.shared_mem==CL_TRUE ? "Shared with CPU" : "Discrete");

	/* Check local memory type */
	cl_device_local_mem_type local_mem_type;
	ret = clGetDeviceInfo(ocl_data.device_id, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(local_mem_type), &local_mem_type, NULL);
	printf("OpenCL device Local Memory type: %s\n", local_mem_type == 1 ? "Local" : "Global");


	/* Create Command Queue */
	const cl_queue_properties properties[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
	ocl_data.command_queue = clCreateCommandQueueWithProperties(ocl_data.context, ocl_data.device_id, properties, &ret);
	if (ret != CL_SUCCESS) {
		printf("OpenCL error clCreateCommandQueueWithProperties %s\n", getErrorString(ret));
		clReleaseContext(ocl_data.context);
		return -1;
	}

	return 0;
}

void ocl_map_buffers() {
	cl_int ret;

	ocl_data.coeff_host = clEnqueueMapBuffer(ocl_data.command_queue,
			ocl_data.coeff_host_clmem, CL_TRUE, CL_MAP_WRITE, 0, ocl_data.buff_size_coeff,
			0, NULL, NULL, &ret);
	if (ret != CL_SUCCESS) {
		printf("Error mapping coeff_host\n");
	}

	ocl_data.src_host = clEnqueueMapBuffer(ocl_data.command_queue,
			ocl_data.src_host_clmem, CL_TRUE, CL_MAP_WRITE, 0, ocl_data.buff_size_src,
			0, NULL, NULL, &ret);
	if (ret != CL_SUCCESS) {
		printf("Error mapping src_host\n");
	}

	ocl_data.dst_host = clEnqueueMapBuffer(ocl_data.command_queue,
			ocl_data.dst_host_clmem, CL_TRUE, CL_MAP_READ, 0, ocl_data.buff_size_dst,
			0, NULL, NULL, &ret);
	if (ret != CL_SUCCESS) {
		printf("Error mapping dst_host\n");
	}
}


void ocl_unmap_buffers(/*cl_event *event*/) {
	cl_int ret;

	/* Unmap source from host */
	ret = clEnqueueUnmapMemObject(
			ocl_data.command_queue,
			ocl_data.coeff_device_clmem,
			ocl_data.coeff_host,
			0,
			NULL,
			NULL);
	if (ret != CL_SUCCESS) {
		printf("Error unmapping coeff_host\n");
	}
	ret = clEnqueueUnmapMemObject(
			ocl_data.command_queue,
			ocl_data.src_device_clmem,
			ocl_data.src_host,
			0,
			NULL,
			NULL);
	if (ret != CL_SUCCESS) {
		printf("Error unmapping src_host\n");
	}
	ret = clEnqueueUnmapMemObject(
			ocl_data.command_queue,
			ocl_data.dst_device_clmem,
			ocl_data.dst_host,
			0,
			NULL,
			NULL);
	if (ret != CL_SUCCESS) {
		printf("Error unmapping dst_host\n");
	}
}

int ocl_alloc_mem(int buff_size_coeff,
		          int buff_size_src, uint8_t **ptr_src,
				  int buff_size_dst, uint8_t **ptr_dst)
{
	cl_int ret;
	ocl_data.coeff_host_clmem  = clCreateBuffer(ocl_data.context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, buff_size_coeff, NULL, &ret);
	if (ret != CL_SUCCESS) {
		printf("Error allocating coeff_host_clmem\n");
	}
	ocl_data.src_host_clmem = clCreateBuffer(ocl_data.context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, buff_size_src, NULL, &ret);
	if (ret != CL_SUCCESS) {
		printf("Error allocating src_host_clmem\n");
	}
	ocl_data.dst_host_clmem = clCreateBuffer(ocl_data.context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, buff_size_dst, NULL, &ret);
	if (ret != CL_SUCCESS) {
		printf("Error allocating dst_host_clmem\n");
	}

	if (ocl_data.shared_mem == CL_TRUE) {
		ocl_data.coeff_device_clmem = ocl_data.coeff_host_clmem;
		ocl_data.src_device_clmem = ocl_data.src_host_clmem;
		ocl_data.dst_device_clmem = ocl_data.dst_host_clmem;
	}
	else {
		ocl_data.coeff_device_clmem  = clCreateBuffer(ocl_data.context, CL_MEM_READ_ONLY, buff_size_coeff, NULL, &ret);
		if (ret != CL_SUCCESS) {
			printf("Error allocating coeff_device_clmem\n");
		}
		ocl_data.src_device_clmem = clCreateBuffer(ocl_data.context, CL_MEM_READ_ONLY, buff_size_src, NULL, &ret);
		if (ret != CL_SUCCESS) {
			printf("Error allocating src_device_clmem\n");
		}
		ocl_data.dst_device_clmem = clCreateBuffer(ocl_data.context, CL_MEM_WRITE_ONLY, buff_size_dst, NULL, &ret);
		if (ret != CL_SUCCESS) {
			printf("Error allocating dst_device_clmem\n");
		}
	}

	ocl_data.buff_size_coeff = buff_size_coeff;
	ocl_data.buff_size_src = buff_size_src;
	ocl_data.buff_size_dst = buff_size_dst;

	//Map all host/shared buffers to host
	ocl_map_buffers();

	*ptr_src = (uint8_t*)(ocl_data.src_host);
	*ptr_dst = (uint8_t*)(ocl_data.dst_host);

	return ret;
}




int ocl_fir2d_run(char *KERNEL_FUNCTION, int kernel_runs,
              int imgHeight, int imgWidth, int imgWidthF,
              int filter_w, int filter_h,
              int8_t *filter_int8, float *filter_f,
			  uint8_t **ptr_dst)
{
	cl_int ret;

	if (strcmp(KERNEL_FUNCTION, "fir2d_kernel_sh_uchar_float")==0 ||
		strcmp(KERNEL_FUNCTION, "fir2d_kernel_sh_float_float")==0 ||
		strcmp(KERNEL_FUNCTION, "fir2d_kernel_sh_float_float_nbc")==0) {
		for (int i=0; i<filter_w*filter_h; i++) {
			*((float*)(ocl_data.coeff_host) + i) = *(filter_f + i);
		}
	}
	else {
		for (int i=0; i<filter_w*filter_h; i++) {
			*((int8_t*)(ocl_data.coeff_host) + i) = *(filter_int8 + i);
		}
	}

	/* Load the source code containing the kernel */
	size_t source_size;
	source_size = KERNEL_STRING.size();

	/* Create Kernel Program from the source */
	ocl_data.program = clCreateProgramWithSource(ocl_data.context, 1, (const char **)&KERNEL_STRING,
		(const size_t *)&source_size, &ret);
	if (ret != CL_SUCCESS) {
		printf("OpenCL error clCreateProgramWithSource %s\n", getErrorString(ret));
		clReleaseCommandQueue(ocl_data.command_queue);
		clReleaseContext(ocl_data.context);
		return -1;
	}

	/* Build Kernel Program */
	ret = clBuildProgram(ocl_data.program, 1, &ocl_data.device_id, NULL, NULL, NULL);
	if (ret != CL_SUCCESS) {
		size_t len;
		char buffer[2048];
		cl_build_status bldstatus;
		printf("\nError %d: Failed to build program executable [ %s ]\n", ret, getErrorString(ret));
		ret = clGetProgramBuildInfo(ocl_data.program, ocl_data.device_id, CL_PROGRAM_BUILD_STATUS, sizeof(bldstatus), (void *)&bldstatus, &len);
		printf("Build Status %d: %s\n", ret, getErrorString(ret));
		printf("INFO: %s\n", getErrorString(bldstatus));
		ret = clGetProgramBuildInfo(ocl_data.program, ocl_data.device_id, CL_PROGRAM_BUILD_OPTIONS, sizeof(buffer), buffer, &len);
		printf("Build Options %d: %s\n", ret, getErrorString(ret));
		printf("INFO: %s\n", buffer);
		ret = clGetProgramBuildInfo(ocl_data.program, ocl_data.device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		printf("Build Log %d: %s\n", ret, getErrorString(ret));
		printf("%s\n", buffer);

		clReleaseProgram(ocl_data.program);
		clReleaseCommandQueue(ocl_data.command_queue);
		clReleaseContext(ocl_data.context);
		return -1;
	}
	printf("OpenCL kernel compiled %ld\n", source_size);

	/* Create OpenCL Kernel */
	ocl_data.kernel = clCreateKernel(ocl_data.program, KERNEL_FUNCTION, &ret);
	if (ret != CL_SUCCESS) {
		printf("OpenCL error clCreateKernel %s %d\n", KERNEL_FUNCTION, ret);
	    clReleaseProgram(ocl_data.program);
	    clReleaseCommandQueue(ocl_data.command_queue);
	    clReleaseContext(ocl_data.context);
		return -1;
	}



	double ts_start, ts_start_k, ts_end, ts_end_k;

	ts_start = get_ts_ns();

	ret = clSetKernelArg(ocl_data.kernel, 0, sizeof(int), &imgWidth);
	ret = clSetKernelArg(ocl_data.kernel, 1, sizeof(int), &imgWidthF);
	ret = clSetKernelArg(ocl_data.kernel, 2, sizeof(int), &filter_w);
	ret = clSetKernelArg(ocl_data.kernel, 3, sizeof(int), &filter_h);
	ret = clSetKernelArg(ocl_data.kernel, 4, sizeof(ocl_data.coeff_device_clmem), (void *)&ocl_data.coeff_device_clmem);
	ret = clSetKernelArg(ocl_data.kernel, 5, sizeof(ocl_data.src_device_clmem), (void *)&ocl_data.src_device_clmem);
	ret = clSetKernelArg(ocl_data.kernel, 6, sizeof(ocl_data.dst_device_clmem), (void *)&ocl_data.dst_device_clmem);

	size_t local_size[3];
	size_t global_size[3];
	if (strcmp(KERNEL_FUNCTION, "fir2d_kernel_gl_16x")==0)
	{
		local_size[0] = 16;
		local_size[1] = 16;
		local_size[2] = 1;
		global_size[0] = (imgWidth * 3) / 16;
		global_size[0] = (global_size[0] + local_size[0] - 1) & ~(local_size[0]-1);
		global_size[1] = imgHeight;
		global_size[2] = 1;

	}
	else if (strcmp(KERNEL_FUNCTION, "fir2d_kernel_sh_float_float_nbc")==0)
	{
		local_size[0] = 32;
		local_size[1] = 8;
		local_size[2] = 1;
		global_size[0] = imgWidth;
		global_size[1] = imgHeight;
		global_size[2] = 1;
	}
	else {
		local_size[0] = 16;
		local_size[1] = 16;
		local_size[2] = 1;
		global_size[0] = imgWidth;
		global_size[1] = imgHeight;
		global_size[2] = 1;
	}
	printf("OpenCL WG: %ldx%ldx%ld, WI: %ldx%ldx%ld\n", global_size[2], global_size[1], global_size[0], local_size[2], local_size[1], local_size[0]);

	if (ocl_data.shared_mem == CL_TRUE) {
		ocl_unmap_buffers();
	}
	else {
		ret = clEnqueueWriteBuffer(ocl_data.command_queue, ocl_data.coeff_device_clmem, CL_TRUE, 0,
				ocl_data.buff_size_coeff, ocl_data.coeff_host, 0, NULL, NULL);
		ret = clEnqueueWriteBuffer(ocl_data.command_queue, ocl_data.src_device_clmem, CL_TRUE, 0,
				ocl_data.buff_size_src, ocl_data.src_host, 0, NULL, NULL);
	}

	ts_start_k = get_ts_ns();
	for (int run = 0; run < kernel_runs; run++)
	{
		ret = clEnqueueNDRangeKernel(ocl_data.command_queue, ocl_data.kernel, 3, NULL, global_size, local_size, 0, NULL, NULL);
	}

	clFinish(ocl_data.command_queue);
	ts_end_k = get_ts_ns();
	if (ret != CL_SUCCESS) {
		printf("OpenCL error clEnqueueNDRangeKernel %s\n", getErrorString(ret));
	}

	if (ocl_data.shared_mem == CL_TRUE) {
		/* Map all buffers to host */
		ocl_map_buffers();
	}
	else {
		ret = clEnqueueReadBuffer(ocl_data.command_queue, ocl_data.dst_device_clmem, CL_TRUE, 0,
			                   	      ocl_data.buff_size_dst, ocl_data.dst_host, 0, NULL, NULL);
	}

	ts_end = get_ts_ns();

	if (ret != CL_SUCCESS) {
		printf("OpenCL error clEnqueueReadBuffer %s\n", getErrorString(ret));
	}

	*ptr_dst = (uint8_t*)(ocl_data.dst_host);

	double elapsed = ts_end - ts_start;
	double elapsed_k = (ts_end_k - ts_start_k)/double(kernel_runs);
	double perf = 1000.0*double(imgHeight*imgWidth)/elapsed_k;
	printf("OpenCL %s runs: %d, total time: %f ms, single kernel time: %f ms, MPixel/s: %f\n",
			KERNEL_FUNCTION, kernel_runs, (elapsed/1000000.0), (elapsed_k/1000000.0), perf);

	return 0;
}

int ocl_release()
{
	cl_int ret;

	ret = clReleaseMemObject(ocl_data.coeff_device_clmem);
	ret = clReleaseMemObject(ocl_data.src_device_clmem);
	ret = clReleaseMemObject(ocl_data.dst_device_clmem);

	if (ocl_data.shared_mem == CL_FALSE) {
		clReleaseMemObject(ocl_data.coeff_host_clmem);
		clReleaseMemObject(ocl_data.src_host_clmem);
		clReleaseMemObject(ocl_data.dst_host_clmem);
	}

	clReleaseKernel(ocl_data.kernel);
    clReleaseProgram(ocl_data.program);
    clReleaseCommandQueue(ocl_data.command_queue);
    clReleaseContext(ocl_data.context);

	return 0;
}



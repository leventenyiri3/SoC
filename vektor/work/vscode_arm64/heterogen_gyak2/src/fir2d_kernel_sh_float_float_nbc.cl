STRINGIFY(

		
		
__kernel void fir2d_kernel_sh_float_float_nbc(
		int imgWidth,
        int imgWidthF,
        int filter_w,
        int filter_h,
		__constant float *filter_coeffs,
		__global unsigned char* gInput,
		__global unsigned char* gOutput
)
{





}



// 4 samples per thread
__kernel void fir2d_kernel_sh_float_float_nbc2(
		int imgWidth,
        int imgWidthF,
        int filter_w,
        int filter_h,
		__constant float *filter_coeffs,
		__global unsigned char* gInput,
		__global unsigned char* gOutput
)
{



}


// 2x4 samples
__kernel void fir2d_kernel_sh_float_float_nbc3(
		int imgWidth,
        int imgWidthF,
        int filter_w,
        int filter_h,
		__constant float *filter_coeffs,
		__global unsigned char* gInput,
		__global unsigned char* gOutput
)
{




}





)
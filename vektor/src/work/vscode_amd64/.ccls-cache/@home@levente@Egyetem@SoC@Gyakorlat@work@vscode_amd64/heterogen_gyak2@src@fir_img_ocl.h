#ifndef _FIR_IMG_OCL_H_
#define _FIR_IMG_OCL_H_


int ocl_init(int ocl_dev);

int ocl_alloc_mem(int buff_size_coeff,
		          int buff_size_src, uint8_t **ptr_src,
				  int buff_size_dst, uint8_t **ptr_dst);

int ocl_fir2d_run(char *KERNEL_FUNCTION, int kernel_runs,
              int imgHeight, int imgWidth, int imgWidthF,
              int filter_w, int filter_h,
              int8_t *filter_int8, float *filter_f,
			  uint8_t **ptr_dst);

int ocl_release();

#endif


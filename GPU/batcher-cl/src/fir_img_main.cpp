#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <chrono>
#include <ctime>

#include <math.h>

#include <omp.h>

#include <IL/ilut.h>
#include <IL/ilu.h>
#include "filter.h"
#include "timestamp.h"
#include "fir_img_ocl.h"

#define FILTER_W 5
#define FILTER_H 5

#ifdef _MSC_VER
	#define memalign(a, s) _aligned_malloc((s), (a))
	#define memfree(a) _aligned_free((a))
#else
	#define memfree(a) free((a))
#endif



void fir2d_c(int32_t imgHeight, int32_t imgWidth, int32_t imgWidthF,
			   int8_t *filter, uint8_t *imgSrcExt, uint8_t *imgDst);


#ifdef __aarch64__
void fir2d_neon_uload(int32_t imgHeight, int32_t imgWidth, int32_t imgWidthF,
		   int8_t *filter, uint8_t *imgSrcExt, uint8_t *imgDst);

void fir2d_neon_sh(int32_t imgHeight, int32_t imgWidth, int32_t imgWidthF,
		   int8_t *filter, uint8_t *imgSrcExt, uint8_t *imgDst);
#endif

#ifdef __amd64__
void fir2d_avx_uload(int32_t imgHeight, int32_t imgWidth, int32_t imgWidthF,
		   int8_t *filter, uint8_t *imgSrcExt, uint8_t *imgDst);
#endif


int32_t main(int32_t argc, char *argv[])
{
	int32_t number_of_runs_scalar, number_of_runs_vector, number_of_runs_kernel;

	if (argc != 6) {
		printf("Usage:\nfir-img 'input filename' 'output filename' 'scalar runs' ' vector runs' 'kernel runs'\nExiting.\n");
		return -1;
	}
	else {
		printf("Input file: %s\n", argv[1]);
		printf("Output file: %s\n", argv[2]);
		number_of_runs_scalar = strtol(argv[3], NULL, 10);
		number_of_runs_vector = strtol(argv[4], NULL, 10);
		number_of_runs_kernel = strtol(argv[5], NULL, 10);
	}

	///////////////////////////////////////////
	// Load input image
	ilInit(); iluInit();
	ILboolean ret;
	ILuint ilImg = 0;
	ilGenImages(1, &ilImg);
	ilBindImage(ilImg);
	ret = ilLoadImage((ILconst_string)(argv[1]));
	if (!ret) {
		printf("Error opening input image, exiting.\n");
		return -1;
	}
	ILubyte* imgData = ilGetData();

	int32_t imgWidth = ilGetInteger(IL_IMAGE_WIDTH);
	int32_t imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
	ILint imgOrigin = ilGetInteger(IL_ORIGIN_MODE);

	printf("Input resolution: %dx%d\n", imgWidth, imgHeight);

	///////////////////////////////////////////
	// Extend input image with zeros
	int32_t imgWidthF = imgWidth + FILTER_W - 1;
	//imgWidthF = (imgWidthF+31) & (~31);		// N*32 byte
	int32_t imgHeightF = imgHeight + FILTER_H - 1;
	int32_t imgFOfssetW = (FILTER_W - 1) / 2;
	int32_t imgFOfssetH = (FILTER_H - 1) / 2;

	/* Filter extended to N*16*/
	int32_t buff_size_coeff = ((FILTER_W*FILTER_H + 15) & ~0xF) * sizeof(float);
	int32_t buff_size_src = 3 * imgWidthF * imgHeightF * sizeof(uint8_t);
	int32_t buff_size_dst = 3 * imgWidth * imgHeight * sizeof(uint8_t);
	printf("Buff size: %d, %d, %d\n", buff_size_coeff, buff_size_src, buff_size_dst);

	/* Init OpenCL */
	ocl_init(-1);

	/* Allocate pinned memory by OpenCL driver and map to host */
	uint8_t *imgSrcExt;
	uint8_t* imgRes;
	ocl_alloc_mem(buff_size_coeff,
			      buff_size_src, &imgSrcExt,
			      buff_size_dst, &imgRes);
	printf("OCL memory allocation done %08lx %08lx\n", (uint64_t)(imgSrcExt), (uint64_t)(imgRes));


	/* Fill extended source image */
	int32_t row, col;
	for (row = 0; row < imgHeightF; row++)
	{
		for (col = 0; col < imgWidthF; col++)
		{
			int pixel = (row * imgWidthF + col) * 3;
			*(imgSrcExt + pixel + 0) = 0;
			*(imgSrcExt + pixel + 1) = 0;
			*(imgSrcExt + pixel + 2) = 0;
		}
	}

	for (row = 0; row < imgHeight; row++)
	{
		for (col = 0; col < imgWidth; col++)
		{
			int pixel_dst = ((row + imgFOfssetH) * imgWidthF + (col + imgFOfssetW)) * 3;
			int pixel_src = (row * imgWidth + col) * 3;
			*(imgSrcExt + pixel_dst + 0) = (uint8_t)(*(imgData + pixel_src + 0));
			*(imgSrcExt + pixel_dst + 1) = (uint8_t)(*(imgData + pixel_src + 1));
			*(imgSrcExt + pixel_dst + 2) = (uint8_t)(*(imgData + pixel_src + 2));
		}
	}
	printf("imgSrcExt generated\n");


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	double ts_start, ts_end, elapsed, perf;



#if 1
	ocl_batcher_run("median_filter_batcher", number_of_runs_kernel,
	              imgHeight, imgWidth, imgWidthF,
				  		  &imgRes);
#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Write output image
	for (row = 0; row < imgHeight; row++)
	{
		for (col = 0; col < imgWidth; col++)
		{
			int pixel_src = (row * imgWidth + col) * 3;
			int pixel_dst = (row * imgWidth + col) * 3;
			*(imgData + pixel_dst + 0) = (ILubyte)(*(imgRes + pixel_src + 0));
			*(imgData + pixel_dst + 1) = (ILubyte)(*(imgRes + pixel_src + 1));
			*(imgData + pixel_dst + 2) = (ILubyte)(*(imgRes + pixel_src + 2));
		}
	}

	ret = ilSetData(imgData);
	ilEnable(IL_FILE_OVERWRITE);

	ilSaveImage((ILconst_string)(argv[2]));

	ilDeleteImages(1, &ilImg);

	ocl_release();

	printf("fir-img done.\n");
	return 0;

}

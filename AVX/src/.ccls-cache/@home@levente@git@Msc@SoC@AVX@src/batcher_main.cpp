#include "batcher.h"

#include <stdint.h>
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

#include "timestamp.h"
#include "filter.h"


#ifdef _MSC_VER
	#define memalign(a, s) _aligned_malloc((s), (a))
	#define memfree(a) _aligned_free((a))
#else
	#define memfree(a) free((a))
#endif



#ifdef __aarch64__
void fir2d_neon_uload(int32_t imgHeight, int32_t imgWidth, int32_t imgWidthF,
		   int8_t *filter, uint8_t *imgSrcExt, uint8_t *imgDst);

void fir2d_neon_sh(int32_t imgHeight, int32_t imgWidth, int32_t imgWidthF,
		   int8_t *filter, uint8_t *imgSrcExt, uint8_t *imgDst);

void fir2d_neon_sh_2x2(int32_t imgHeight, int32_t imgWidth, int32_t imgWidthF,
		   int8_t *filter, uint8_t *imgSrcExt, uint8_t *imgDst);
#endif

#ifdef __amd64__


void media_filter_vector(int imgHeight, int imgWidth, int imgWidthF,
			   uint8_t *imgSrcExt, uint8_t *imgDst);
#endif

int main(int argc, char *argv[])
{

	if (argc != 5) {
		printf("Usage:\nfir-img 'input filename' 'output filename' '#of runs scalar' '#of runs vector'\nExiting.\n");
		return -1;
	}
	else {
		printf("Input file: %s\n", argv[1]);
		printf("Output file: %s\n", argv[2]);
	}
	int32_t number_of_runs_scalar, number_of_runs_vector;
	number_of_runs_scalar = strtol(argv[3], NULL, 10);
	number_of_runs_vector = strtol(argv[4], NULL, 10);

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

	printf("Input resolution: %dx%d\n", imgWidth, imgHeight);

	///////////////////////////////////////////
	// Extend input image with zeros
	int32_t imgWidthF = imgWidth + 5 - 1;
	imgWidthF = (imgWidthF+31) & (~31);		// N*32 byte
	int32_t imgHeightF = imgHeight + 5 - 1;
	int32_t imgFOfssetW = (5 - 1) / 2;
	int32_t imgFOfssetH = (5 - 1) / 2;

	/* Filter extended to N*16*/
	int32_t buff_size_coeff = ((5*5 + 15) & ~0xF) * sizeof(float);
	int32_t buff_size_src = 3 * imgWidthF * imgHeightF * sizeof(uint8_t);
	int32_t buff_size_dst = 3 * imgWidth * imgHeight * sizeof(uint8_t);
	printf("Buff size: %d, %d, %d\n", buff_size_coeff, buff_size_src, buff_size_dst);


	uint8_t *imgSrcExt;
	imgSrcExt = (uint8_t*)(memalign(4096, buff_size_src));

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


	uint8_t* imgRes;
	imgRes = (uint8_t*)(memalign(4096, buff_size_dst));


	int8_t filter_laplace_int8[5*5];
	for (int32_t row=0; row<5; row++) {
		for (int32_t col=0; col<5; col++) {
			filter_laplace_int8[row*5 + col] = (int8_t)(filter_laplace_f[row*5 + col]);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	double ts_start, ts_end, elapsed, perf;


#ifdef __amd64__
#if 1
	ts_start = get_ts_ns();
	for (int32_t run=0; run<number_of_runs_vector; run++)
	{
		media_filter_vector(imgHeight,
				imgWidth,
				imgWidthF,
				imgSrcExt,
				imgRes);
	}
	ts_end = get_ts_ns();
	elapsed = ts_end - ts_start;

	perf = 1000.0*double(imgHeight*imgWidth)*double(number_of_runs_vector)/elapsed;
	printf("CPU AVX_ULOAD time: %f ms, MPixels/s: %f\n", (elapsed/1000000.0), perf);
#endif
#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Write output images
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

	memfree(imgRes);



	printf("fir-img done.\n");
	return 0;

}

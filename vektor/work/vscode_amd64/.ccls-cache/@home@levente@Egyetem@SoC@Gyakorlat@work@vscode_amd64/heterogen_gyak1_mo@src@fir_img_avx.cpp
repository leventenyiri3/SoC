#ifdef __amd64__

#include <stdint.h>

#include <omp.h>

#include <emmintrin.h>
#include <nmmintrin.h>
#include <immintrin.h>




void fir2d_avx_uload(int32_t imgHeight, int32_t imgWidth, int32_t imgWidthF,
		   int8_t *filter, uint8_t *imgSrcExt, uint8_t *imgDst)
{
	// Generate the vector-coefficients
	__m256i filter_laplace[25];
	for (int32_t i = 0; i < 25; i++)
	{
		filter_laplace[i] = _mm256_set1_epi16((int16_t)(filter[i]));
	}

	#pragma omp parallel for
	for (int32_t row=0; row<imgHeight; row++)
	{
		int32_t wr_base = row*imgWidth*3;
		int32_t rd_base = row*imgWidthF*3;

		// step through all components in a line
		for (int32_t col=0; col<(imgWidth*3); col=col+16)
		{
			__m256i fval;
			fval = _mm256_set1_epi16(0);

			int32_t rd_offset = 0;
			for (int32_t fy=0; fy<5; fy++)
			{
				for (int32_t fx=0; fx<5; fx++)
				{
					int32_t pixel_src = rd_base + rd_offset;
					__m128i load_data;
					__m256i conv_res, mul_res;

					load_data = _mm_lddqu_si128((__m128i *)(imgSrcExt + pixel_src));
					conv_res = _mm256_cvtepu8_epi16(load_data);
					mul_res = _mm256_mullo_epi16(conv_res, filter_laplace[fy * 5 + fx]);
					fval = _mm256_add_epi16(fval, mul_res);
					rd_offset = rd_offset + 3;

				}
				rd_offset = rd_offset - 5*3 + imgWidthF*3;
			}
			
			__m256i sat_val;
			sat_val = _mm256_set1_epi16(255);
			fval = _mm256_min_epi16(fval, sat_val);
			sat_val = _mm256_set1_epi16(0);
			fval = _mm256_max_epi16(fval, sat_val);
			
			for (int32_t i = 0; i < 16; i++)
			{
				*(imgDst + wr_base + i) = (uint8_t)(_mm256_extract_epi16 (fval, i));
			}

			wr_base = wr_base + 16;
			rd_base = rd_base + 16;
		}
	}
}

#endif
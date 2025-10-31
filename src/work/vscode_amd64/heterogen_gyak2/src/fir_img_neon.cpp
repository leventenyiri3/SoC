#ifdef __aarch64__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>

#include <arm_neon.h>


void fir2d_neon_uload(int32_t imgHeight, int32_t imgWidth, int32_t imgWidthF,
		   int8_t *filter, uint8_t *imgSrcExt, uint8_t *imgDst)
{

	// Generate the vector-coefficients
	int16x8_t filter_laplace[25];
	for (int32_t i = 0; i < 25; i++)
	{
		filter_laplace[i] = vdupq_n_s16(int16_t(filter[i]));
	}

#pragma omp parallel for
	for (int32_t row=0; row<imgHeight; row++)
	{
		for (int32_t col=0; col<(imgWidth*3); col=col+8)
		{
			int32_t rd_offset;
			uint8x8_t load_data;
			int16x8_t ext_data;
			int16x8_t coeff;
			int16x8_t accu;

			accu = vdupq_n_s16(0);
			for (int32_t fy=0; fy<5; fy++)
			{
				for (int32_t fx=0; fx<5; fx++)
				{
					rd_offset = (row+fy)*imgWidthF*3 + col + fx*3;
					load_data = vld1_u8(imgSrcExt + rd_offset);
					ext_data = vreinterpretq_s16_u16( vmovl_u8(load_data) );
					coeff = filter_laplace[fy * 5 + fx];
					accu = vmlaq_s16(accu, coeff, ext_data);
				}
			}
			int16x8_t sat, v_0;
			v_0 = vdupq_n_s16(0);
			sat = vmaxq_s16(accu, v_0);
			uint8x8_t res;
			res = vqmovn_u16( vreinterpretq_u16_s16(sat) );

			int32_t wr_offset;
			wr_offset = row*imgWidth*3 + col;
			vst1_u8(imgDst + wr_offset, res);
		}
	}
}






void fir2d_neon_sh(int32_t imgHeight, int32_t imgWidth, int32_t imgWidthF,
		   int8_t *filter, uint8_t *imgSrcExt, uint8_t *imgDst)
{

	// Generate the vector-coefficients
	int16x8_t filter_laplace[25];
	for (int32_t i = 0; i < 25; i++)
	{
		filter_laplace[i] = vdupq_n_s16(int16_t(filter[i]));
	}

#pragma omp parallel for
	for (int32_t row=0; row<imgHeight; row++)
	{
		for (int32_t col=0; col<(imgWidth*3); col=col+8)
		{
			int32_t rd_offset;
			uint8x8_t load_data;
			int16x8_t ext_data[3];
			int16x8_t smpl;
			int16x8_t coeff;
			int16x8_t accu;

			accu = vdupq_n_s16(0);
			for (int32_t fy=0; fy<5; fy++)
			{
				rd_offset = (row+fy)*imgWidthF*3 + col;
				load_data = vld1_u8(imgSrcExt + rd_offset + 0);
				ext_data[0] = vreinterpretq_s16_u16( vmovl_u8(load_data) );
				load_data = vld1_u8(imgSrcExt + rd_offset + 8);
				ext_data[1] = vreinterpretq_s16_u16( vmovl_u8(load_data) );
				load_data = vld1_u8(imgSrcExt + rd_offset + 16);
				ext_data[2] = vreinterpretq_s16_u16( vmovl_u8(load_data) );

				smpl = ext_data[0];
				coeff = filter_laplace[fy * 5 + 0];
				accu = vmlaq_s16(accu, coeff, smpl);

				smpl = vextq_s16(ext_data[0], ext_data[1], 3);
				coeff = filter_laplace[fy * 5 + 1];
				accu = vmlaq_s16(accu, coeff, smpl);

				smpl = vextq_s16(ext_data[0], ext_data[1], 6);
				coeff = filter_laplace[fy * 5 + 2];
				accu = vmlaq_s16(accu, coeff, smpl);

				smpl = vextq_s16(ext_data[1], ext_data[2], 1);
				coeff = filter_laplace[fy * 5 + 3];
				accu = vmlaq_s16(accu, coeff, smpl);

				smpl = vextq_s16(ext_data[1], ext_data[2], 4);
				coeff = filter_laplace[fy * 5 + 4];
				accu = vmlaq_s16(accu, coeff, smpl);
			}
			int16x8_t sat, v_0;
			v_0 = vdupq_n_s16(0);
            sat = vmaxq_s16(accu, v_0);
			uint8x8_t res;
			res = vqmovn_u16( vreinterpretq_u16_s16(sat) );

			int32_t wr_offset;
			wr_offset = row*imgWidth*3 + col;
			vst1_u8(imgDst + wr_offset, res);
		}
	}
}



void fir2d_neon_sh_2x2(int32_t imgHeight, int32_t imgWidth, int32_t imgWidthF,
		   int8_t *filter, uint8_t *imgSrcExt, uint8_t *imgDst)
{

	// Generate the vector-coefficients
	int16x8_t filter_laplace[25];
	for (int32_t i = 0; i < 25; i++)
	{
		filter_laplace[i] = vdupq_n_s16(int16_t(filter[i]));
	}

#pragma omp parallel for
	for (int32_t row=0; row<imgHeight; row=row+2)
	{
		for (int32_t col=0; col<(imgWidth*3); col=col+2*8)
		{
			int32_t rd_offset;
			uint8x8_t load_data;
			int16x8_t ext_data[4];
			int16x8_t smpl[2];
			int16x8_t coeff[2];
			int16x8_t accu[2][2];

			accu[0][0] = vdupq_n_s16(0);
            accu[0][1] = vdupq_n_s16(0);
            accu[1][0] = vdupq_n_s16(0);
            accu[1][1] = vdupq_n_s16(0);
			
            {
				rd_offset = (row)*imgWidthF*3 + col;
				load_data = vld1_u8(imgSrcExt + rd_offset + 0);
				ext_data[0] = vreinterpretq_s16_u16( vmovl_u8(load_data) );
				load_data = vld1_u8(imgSrcExt + rd_offset + 8);
				ext_data[1] = vreinterpretq_s16_u16( vmovl_u8(load_data) );
				load_data = vld1_u8(imgSrcExt + rd_offset + 16);
				ext_data[2] = vreinterpretq_s16_u16( vmovl_u8(load_data) );
                load_data = vld1_u8(imgSrcExt + rd_offset + 24);
				ext_data[3] = vreinterpretq_s16_u16( vmovl_u8(load_data) );

                coeff[0] = filter_laplace[0 * 5 + 0];
				smpl[0] = ext_data[0];
                smpl[1] = ext_data[1];
				accu[0][0] = vmlaq_s16(accu[0][0], coeff[0], smpl[0]);
                accu[0][1] = vmlaq_s16(accu[0][1], coeff[0], smpl[1]);

				coeff[0] = filter_laplace[0 * 5 + 1];
				smpl[0] = vextq_s16(ext_data[0], ext_data[1], 3);
                smpl[1] = vextq_s16(ext_data[1], ext_data[2], 3);
				accu[0][0] = vmlaq_s16(accu[0][0], coeff[0], smpl[0]);
                accu[0][1] = vmlaq_s16(accu[0][1], coeff[0], smpl[1]);

				coeff[0] = filter_laplace[0 * 5 + 2];
                smpl[0] = vextq_s16(ext_data[0], ext_data[1], 6);
				smpl[1] = vextq_s16(ext_data[1], ext_data[2], 6);
                accu[0][0] = vmlaq_s16(accu[0][0], coeff[0], smpl[0]);
                accu[0][1] = vmlaq_s16(accu[0][1], coeff[0], smpl[1]);

				coeff[0] = filter_laplace[0 * 5 + 3];
                smpl[0] = vextq_s16(ext_data[1], ext_data[2], 1);
				smpl[1] = vextq_s16(ext_data[2], ext_data[3], 1);
                accu[0][0] = vmlaq_s16(accu[0][0], coeff[0], smpl[0]);
                accu[0][1] = vmlaq_s16(accu[0][1], coeff[0], smpl[1]);

                coeff[0] = filter_laplace[0 * 5 + 4];
				smpl[0] = vextq_s16(ext_data[1], ext_data[2], 4);
                smpl[1] = vextq_s16(ext_data[2], ext_data[3], 4);
				accu[0][0] = vmlaq_s16(accu[0][0], coeff[0], smpl[0]);
                accu[0][1] = vmlaq_s16(accu[0][1], coeff[0], smpl[1]);                
            }
            
            for (int32_t fy=1; fy<5; fy++)
			{
				rd_offset = (row+fy)*imgWidthF*3 + col;
				load_data = vld1_u8(imgSrcExt + rd_offset + 0);
				ext_data[0] = vreinterpretq_s16_u16( vmovl_u8(load_data) );
				load_data = vld1_u8(imgSrcExt + rd_offset + 8);
				ext_data[1] = vreinterpretq_s16_u16( vmovl_u8(load_data) );
				load_data = vld1_u8(imgSrcExt + rd_offset + 16);
				ext_data[2] = vreinterpretq_s16_u16( vmovl_u8(load_data) );
                load_data = vld1_u8(imgSrcExt + rd_offset + 24);
				ext_data[3] = vreinterpretq_s16_u16( vmovl_u8(load_data) );


                coeff[0] = filter_laplace[(fy) * 5 + 0];
                coeff[1] = filter_laplace[(fy-1) * 5 + 0];
				smpl[0] = ext_data[0];
                smpl[1] = ext_data[1];
				accu[0][0] = vmlaq_s16(accu[0][0], coeff[0], smpl[0]);
                accu[0][1] = vmlaq_s16(accu[0][1], coeff[0], smpl[1]);
                accu[1][0] = vmlaq_s16(accu[1][0], coeff[1], smpl[0]);
                accu[1][1] = vmlaq_s16(accu[1][1], coeff[1], smpl[1]);

                coeff[0] = filter_laplace[(fy) * 5 + 1];
                coeff[1] = filter_laplace[(fy-1) * 5 + 1];
				smpl[0] = vextq_s16(ext_data[0], ext_data[1], 3);
                smpl[1] = vextq_s16(ext_data[1], ext_data[2], 3);
				coeff[0] = filter_laplace[0 * 5 + 1];
				accu[0][0] = vmlaq_s16(accu[0][0], coeff[0], smpl[0]);
                accu[0][1] = vmlaq_s16(accu[0][1], coeff[0], smpl[1]);
                accu[1][0] = vmlaq_s16(accu[1][0], coeff[1], smpl[0]);
                accu[1][1] = vmlaq_s16(accu[1][1], coeff[1], smpl[1]);

				coeff[0] = filter_laplace[(fy) * 5 + 2];
                coeff[1] = filter_laplace[(fy-1) * 5 + 2];
                smpl[0] = vextq_s16(ext_data[0], ext_data[1], 6);
				smpl[1] = vextq_s16(ext_data[1], ext_data[2], 6);
                accu[0][0] = vmlaq_s16(accu[0][0], coeff[0], smpl[0]);
                accu[0][1] = vmlaq_s16(accu[0][1], coeff[0], smpl[1]);
                accu[1][0] = vmlaq_s16(accu[1][0], coeff[1], smpl[0]);
                accu[1][1] = vmlaq_s16(accu[1][1], coeff[1], smpl[1]);

				coeff[0] = filter_laplace[(fy) * 5 + 3];
                coeff[1] = filter_laplace[(fy-1) * 5 + 3];
                smpl[0] = vextq_s16(ext_data[1], ext_data[2], 1);
				smpl[1] = vextq_s16(ext_data[2], ext_data[3], 1);
                accu[0][0] = vmlaq_s16(accu[0][0], coeff[0], smpl[0]);
                accu[0][1] = vmlaq_s16(accu[0][1], coeff[0], smpl[1]);
                accu[1][0] = vmlaq_s16(accu[1][0], coeff[1], smpl[0]);
                accu[1][1] = vmlaq_s16(accu[1][1], coeff[1], smpl[1]);

                coeff[0] = filter_laplace[(fy) * 5 + 4];
                coeff[1] = filter_laplace[(fy-1) * 5 + 4];
				smpl[0] = vextq_s16(ext_data[1], ext_data[2], 4);
                smpl[1] = vextq_s16(ext_data[2], ext_data[3], 4);
				accu[0][0] = vmlaq_s16(accu[0][0], coeff[0], smpl[0]);
                accu[0][1] = vmlaq_s16(accu[0][1], coeff[0], smpl[1]);
                accu[1][0] = vmlaq_s16(accu[1][0], coeff[1], smpl[0]);
                accu[1][1] = vmlaq_s16(accu[1][1], coeff[1], smpl[1]); 
			}

			{
				rd_offset = (row+5)*imgWidthF*3 + col;
				load_data = vld1_u8(imgSrcExt + rd_offset + 0);
				ext_data[0] = vreinterpretq_s16_u16( vmovl_u8(load_data) );
				load_data = vld1_u8(imgSrcExt + rd_offset + 8);
				ext_data[1] = vreinterpretq_s16_u16( vmovl_u8(load_data) );
				load_data = vld1_u8(imgSrcExt + rd_offset + 16);
				ext_data[2] = vreinterpretq_s16_u16( vmovl_u8(load_data) );
                load_data = vld1_u8(imgSrcExt + rd_offset + 24);
				ext_data[3] = vreinterpretq_s16_u16( vmovl_u8(load_data) );

                coeff[1] = filter_laplace[(4) * 5 + 0];
				smpl[0] = ext_data[0];
                smpl[1] = ext_data[1];
                accu[1][0] = vmlaq_s16(accu[1][0], coeff[1], smpl[0]);
                accu[1][1] = vmlaq_s16(accu[1][1], coeff[1], smpl[1]);

                coeff[1] = filter_laplace[(4) * 5 + 1];
				smpl[0] = vextq_s16(ext_data[0], ext_data[1], 3);
                smpl[1] = vextq_s16(ext_data[1], ext_data[2], 3);
                accu[1][0] = vmlaq_s16(accu[1][0], coeff[1], smpl[0]);
                accu[1][1] = vmlaq_s16(accu[1][1], coeff[1], smpl[1]);

                coeff[1] = filter_laplace[(4) * 5 + 2];
                smpl[0] = vextq_s16(ext_data[0], ext_data[1], 6);
				smpl[1] = vextq_s16(ext_data[1], ext_data[2], 6);
                accu[1][0] = vmlaq_s16(accu[1][0], coeff[1], smpl[0]);
                accu[1][1] = vmlaq_s16(accu[1][1], coeff[1], smpl[1]);

                coeff[1] = filter_laplace[(4) * 5 + 3];
                smpl[0] = vextq_s16(ext_data[1], ext_data[2], 1);
				smpl[1] = vextq_s16(ext_data[2], ext_data[3], 1);
                accu[1][0] = vmlaq_s16(accu[1][0], coeff[1], smpl[0]);
                accu[1][1] = vmlaq_s16(accu[1][1], coeff[1], smpl[1]);

                coeff[1] = filter_laplace[(4) * 5 + 4];
				smpl[0] = vextq_s16(ext_data[1], ext_data[2], 4);
                smpl[1] = vextq_s16(ext_data[2], ext_data[3], 4);
                accu[1][0] = vmlaq_s16(accu[1][0], coeff[1], smpl[0]);
                accu[1][1] = vmlaq_s16(accu[1][1], coeff[1], smpl[1]); 
			}

			int16x8_t sat, v_0;
            uint8x8_t res[2][2];
			v_0 = vdupq_n_s16(0);
			sat = vmaxq_s16(accu[0][0], v_0);
			res[0][0] = vqmovn_u16( vreinterpretq_u16_s16(sat) );
            sat = vmaxq_s16(accu[0][1], v_0);
			res[0][1] = vqmovn_u16( vreinterpretq_u16_s16(sat) );
            sat = vmaxq_s16(accu[1][0], v_0);
			res[1][0] = vqmovn_u16( vreinterpretq_u16_s16(sat) );
            sat = vmaxq_s16(accu[1][1], v_0);
			res[1][1] = vqmovn_u16( vreinterpretq_u16_s16(sat) );

			int32_t wr_offset;
			wr_offset = row*imgWidth*3 + col;
			vst1_u8(imgDst + wr_offset, res[0][0]);
            vst1_u8(imgDst + wr_offset + 8, res[0][1]);
            vst1_u8(imgDst + wr_offset + imgWidth*3, res[1][0]);
            vst1_u8(imgDst + wr_offset + imgWidth*3 + 8, res[1][1]);
		}
	}
}


#endif
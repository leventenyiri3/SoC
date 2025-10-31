#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>

void fir2d_c(int imgHeight, int imgWidth, int imgWidthF,
			   int8_t *filter, uint8_t *imgSrcExt, uint8_t *imgDst)
{

	#pragma omp parallel for
	for (int row=0; row<imgHeight; row++)
	{
		for (int col=0; col<imgWidth; col++)
		{
			short fval[3] = {0,0,0};
			int rd_offset;
			for (int fy=0; fy<5; fy++)
			{
				for (int fx=0; fx<5; fx++)
				{
					rd_offset = ((row+fy)*imgWidthF + (col+fx))*3;
					for (int rgba=0; rgba<3; rgba++)
					{
						fval[rgba] = fval[rgba] + *(filter+fy*5+fx) * (short)(*(imgSrcExt + rd_offset + rgba));
					}
				}
			}

			for (int rgba=0; rgba<3; rgba++)
			{
				if (fval[rgba]>255) fval[rgba]=255;
				if (fval[rgba]<0)   fval[rgba]=0;
			}

			int wr_offset;
			wr_offset = row*imgWidth*3 + col*3;
			for (int rgba=0; rgba<3; rgba++)
			{
				*(imgDst + wr_offset + rgba) = (uint8_t)(fval[rgba]);
			}
		}
	}
}

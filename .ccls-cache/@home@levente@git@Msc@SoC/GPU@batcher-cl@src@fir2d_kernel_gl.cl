STRINGIFY(

// Use an inline function instead of a #define to avoid stringify errors
inline void compare_exchange(unsigned char *a, unsigned char *b) {
    unsigned char lo = min(*a, *b);
    unsigned char hi = max(*a, *b);
    *a = lo;
    *b = hi;
}

__kernel void median_filter_batcher(
    int imgWidth,
    int imgWidthF,
    __global unsigned char* gInput,
    __global unsigned char* gOutput)
{
    int col = get_global_id(0);
    int row = get_global_id(1);

    if (col >= imgWidth || row >= get_global_size(1)) return;

    for (int c = 0; c < 3; c++) {
        unsigned char p[25];

        // 1. Load data
        for (int fy = 0; fy < 5; fy++) {
            for (int fx = 0; fx < 5; fx++) {
                int rd_offset = ((row + fy) * imgWidthF * 3) + (col + fx) * 3 + c;
                p[fy * 5 + fx] = gInput[rd_offset];
            }
        }

      compare_exchange(&p[0],  &p[1]);
      compare_exchange(&p[2],  &p[3]);
      compare_exchange(&p[0],  &p[2]);
      compare_exchange(&p[1],  &p[3]);
      compare_exchange(&p[1],  &p[2]);

      compare_exchange(&p[4],  &p[5]);
      compare_exchange(&p[6],  &p[7]);
      compare_exchange(&p[4],  &p[6]);
      compare_exchange(&p[5],  &p[7]);
      compare_exchange(&p[5],  &p[6]);

      compare_exchange(&p[0],  &p[4]);
      compare_exchange(&p[2],  &p[6]);
      compare_exchange(&p[2],  &p[4]);
      compare_exchange(&p[1],  &p[5]);
      compare_exchange(&p[3],  &p[7]);
      compare_exchange(&p[3],  &p[5]);
      compare_exchange(&p[1],  &p[2]);
      compare_exchange(&p[3],  &p[4]);
      compare_exchange(&p[5],  &p[6]);

      compare_exchange(&p[8],  &p[9]);
      compare_exchange(&p[10], &p[11]);
      compare_exchange(&p[8],  &p[10]);
      compare_exchange(&p[9],  &p[11]);
      compare_exchange(&p[9],  &p[10]);

      compare_exchange(&p[12], &p[13]);
      compare_exchange(&p[14], &p[15]);
      compare_exchange(&p[12], &p[14]);
      compare_exchange(&p[13], &p[15]);
      compare_exchange(&p[13], &p[14]);

      compare_exchange(&p[8],  &p[12]);
      compare_exchange(&p[10], &p[14]);
      compare_exchange(&p[10], &p[12]);
      compare_exchange(&p[9],  &p[13]);
      compare_exchange(&p[11], &p[15]);
      compare_exchange(&p[11], &p[13]);
      compare_exchange(&p[9],  &p[10]);
      compare_exchange(&p[11], &p[12]);
      compare_exchange(&p[13], &p[14]);

      compare_exchange(&p[0],  &p[8]);
      compare_exchange(&p[4],  &p[12]);
      compare_exchange(&p[4],  &p[8]);
      compare_exchange(&p[2],  &p[10]);
      compare_exchange(&p[6],  &p[14]);
      compare_exchange(&p[6],  &p[10]);
      compare_exchange(&p[2],  &p[4]);
      compare_exchange(&p[6],  &p[8]);
      compare_exchange(&p[10], &p[12]);
      compare_exchange(&p[1],  &p[9]);
      compare_exchange(&p[5],  &p[13]);
      compare_exchange(&p[5],  &p[9]);
      compare_exchange(&p[3],  &p[11]);
      compare_exchange(&p[7],  &p[15]);
      compare_exchange(&p[7],  &p[11]);
      compare_exchange(&p[3],  &p[5]);
      compare_exchange(&p[7],  &p[9]);
      compare_exchange(&p[11], &p[13]);
      compare_exchange(&p[1],  &p[2]);
      compare_exchange(&p[3],  &p[4]);
      compare_exchange(&p[5],  &p[6]);
      compare_exchange(&p[7],  &p[8]);
      compare_exchange(&p[9],  &p[10]);
      compare_exchange(&p[11], &p[12]);
      compare_exchange(&p[13], &p[14]);

      compare_exchange(&p[16], &p[17]);
      compare_exchange(&p[18], &p[19]);
      compare_exchange(&p[16], &p[18]);
      compare_exchange(&p[17], &p[19]);
      compare_exchange(&p[17], &p[18]);

      compare_exchange(&p[20], &p[21]);
      compare_exchange(&p[22], &p[23]);
      compare_exchange(&p[20], &p[22]);
      compare_exchange(&p[21], &p[23]);
      compare_exchange(&p[21], &p[22]);

      compare_exchange(&p[16], &p[20]);
      compare_exchange(&p[18], &p[22]);
      compare_exchange(&p[18], &p[20]);
      compare_exchange(&p[17], &p[21]);
      compare_exchange(&p[19], &p[23]);
      compare_exchange(&p[19], &p[21]);
      compare_exchange(&p[17], &p[18]);
      compare_exchange(&p[19], &p[20]);
      compare_exchange(&p[21], &p[22]);

      compare_exchange(&p[20], &p[24]);
      compare_exchange(&p[22], &p[24]);
      compare_exchange(&p[21], &p[24]);
      compare_exchange(&p[23], &p[24]);

      compare_exchange(&p[16], &p[20]);
      compare_exchange(&p[18], &p[22]);
      compare_exchange(&p[18], &p[20]);
      compare_exchange(&p[17], &p[21]);
      compare_exchange(&p[19], &p[23]);
      compare_exchange(&p[19], &p[21]);
      compare_exchange(&p[17], &p[18]);
      compare_exchange(&p[19], &p[20]);
      compare_exchange(&p[21], &p[22]);

      compare_exchange(&p[0],  &p[16]);
      compare_exchange(&p[8],  &p[24]);
      compare_exchange(&p[8],  &p[16]);
      compare_exchange(&p[4],  &p[20]);
      compare_exchange(&p[12], &p[20]);
      compare_exchange(&p[12], &p[16]);
      compare_exchange(&p[2],  &p[18]);
      compare_exchange(&p[10], &p[18]);
      compare_exchange(&p[6],  &p[22]);
      compare_exchange(&p[6],  &p[10]);
      compare_exchange(&p[10], &p[12]);
      compare_exchange(&p[1],  &p[17]);
      compare_exchange(&p[9],  &p[17]);
      compare_exchange(&p[5],  &p[21]);
      compare_exchange(&p[13], &p[21]);
      compare_exchange(&p[13], &p[17]);
      compare_exchange(&p[3],  &p[19]);
      compare_exchange(&p[11], &p[19]);
      compare_exchange(&p[7],  &p[23]);
      compare_exchange(&p[7],  &p[11]);
      compare_exchange(&p[11], &p[13]);
      compare_exchange(&p[11], &p[12]);
        // 3. Output
        int wr_offset = (row * imgWidth + col) * 3 + c;
        gOutput[wr_offset] = p[12];
    }
}
		
)

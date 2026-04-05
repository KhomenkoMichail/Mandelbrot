#ifndef FUNC_MANDELBROT
#define FUNC_MANDELBROT

void _mm256_set1_ps (float arr[8], float a);

void _mm256_set_ps (float arr[8], float e7, float e6, float e5, float e4,
                                  float e3, float e2, float e1, float e0);

void _mm256_add_ps (float dst[8], float a[8], float b[8]);

void _mm256_sub_ps (float dst[8], float a[8], float b[8]);

void _mm256_mul_ps (float dst[8], float a[8], float b[8]);

void _mm256_equ_ps (float dst[8], float src[8]);

void _mm256_cmp_le_ps (int cmp[8], float a[8], float b[8]);

void _mm256_add_epi32 (int dst[8], int a[8], int b[8]);

void _mm256_slli_epi32 (BYTE dst[8], int src[8], int shift);

int _mm256_movemask_epi8 (int cmp[8]);

#endif

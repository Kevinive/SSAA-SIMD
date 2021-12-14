// #include "SSAA.hpp"
#include <immintrin.h>

#define SIMD_FMA(src1, src2, src3, dest) dest = _mm256_fmadd_pd(src1, src2, src3)
#define SIMD_ADD(src1, src2, dest) dest = _mm256_add_pd(src1, src2)
#define SIMD_MUL(src1, src2, dest) dest = _mm256_mul_pd(src1, src2)
#define SIMD_CMP(src1, src2, cmd, dest) dest = _mm256_cmp_pd(src1, src2, cmd)
#define SIMD_AND(src1, src2, dest) dest = _mm256_and_pd(src1, src2)
#define SIMD_LOAD(src1, dest) dest = _mm256_load_pd(src1)
#define SIMD_BROADCAST(src1, dest) dest = _mm256_broadcast_sd(src1)
#define SIMD_STORE(src1, dest) _mm256_store_pd(dest, src1)

#define CMP_CMD 0x02

static const double pix_offsets[] = {0.0, 0.0625, 0.125, 0.1875, 0.25, 0.3125, 0.375, 0.4375,
       0.5, 0.5625, 0.625, 0.6875, 0.75, 0.8125, 0.875 , 0.9375};
static const double zero = 0.0;
static const double frac256 = 0.00390625;

void circle_kernel(double* point_on_canvas, double x0, double y0, 
        double xc, double yc, double r, double color){
    const double *offsetPtr = pix_offsets+1;

    register __m256d ymm0, ymm1, ymm2, ymm3, ymm4, ymm5, ymm6, ymm7;
    register __m256d ymm8, ymm9, ymm10, ymm11, ymm12, ymm13, ymm14, ymm15;

    double result[4];

    x0 -= xc;
    y0 -= yc;
    
    // ymm0-3 for x^2
    SIMD_BROADCAST(&x0, ymm0);
    SIMD_BROADCAST(&x0, ymm1);
    SIMD_BROADCAST(&x0, ymm2);
    SIMD_BROADCAST(&x0, ymm3);
    
    SIMD_LOAD(pix_offsets, ymm6);
    SIMD_LOAD(pix_offsets+4, ymm7);
    SIMD_LOAD(pix_offsets+8, ymm8);
    SIMD_LOAD(pix_offsets+12, ymm9);

    //y for first line
    SIMD_BROADCAST(&y0, ymm4);
    SIMD_BROADCAST(offsetPtr, ymm5);
    // ymm15 for R
    SIMD_BROADCAST(&r, ymm15);
    // ymm14 for color
    SIMD_BROADCAST(&color, ymm14);
    // ymm10-13 for Sum
    SIMD_BROADCAST(&zero, ymm13);
    SIMD_BROADCAST(&zero, ymm12);
    SIMD_BROADCAST(&zero, ymm11);
    SIMD_BROADCAST(&zero, ymm10);

    SIMD_ADD(ymm0, ymm6, ymm0);
    SIMD_ADD(ymm1, ymm7, ymm1);
    SIMD_ADD(ymm2, ymm8, ymm2);
    SIMD_ADD(ymm3, ymm9, ymm3);

    SIMD_MUL(ymm0, ymm0, ymm0);
    SIMD_MUL(ymm1, ymm1, ymm1);
    SIMD_MUL(ymm2, ymm2, ymm2);
    SIMD_MUL(ymm3, ymm3, ymm3);
    
    for(int i = 15; i != 0; i--) {
        SIMD_FMA(ymm4, ymm4, ymm0, ymm6);   // 5 cycles
        SIMD_FMA(ymm4, ymm4, ymm1, ymm7);
        SIMD_FMA(ymm4, ymm4, ymm2, ymm8);
        SIMD_FMA(ymm4, ymm4, ymm3, ymm9);

        SIMD_ADD(ymm4, ymm5, ymm4);     // Update Y

        SIMD_CMP(ymm6, ymm15, CMP_CMD, ymm6);   // Compare with R
        SIMD_CMP(ymm7, ymm15, CMP_CMD, ymm7);
        SIMD_CMP(ymm8, ymm15, CMP_CMD, ymm8);
        SIMD_CMP(ymm9, ymm15, CMP_CMD, ymm9);
        SIMD_AND(ymm6, ymm14, ymm6);   // Mask Color
        SIMD_AND(ymm7, ymm14, ymm7);
        SIMD_AND(ymm8, ymm14, ymm8);
        SIMD_AND(ymm9, ymm14, ymm9);

        SIMD_ADD(ymm6, ymm10, ymm10);
        SIMD_ADD(ymm7, ymm11, ymm11);
        SIMD_ADD(ymm8, ymm12, ymm12);
        SIMD_ADD(ymm9, ymm13, ymm13);
    }

    SIMD_BROADCAST(&frac256, ymm5);    // Broadcast Frac 1/256

    // Final Round
    SIMD_FMA(ymm4, ymm4, ymm0, ymm6);
    SIMD_FMA(ymm4, ymm4, ymm1, ymm7);
    SIMD_FMA(ymm4, ymm4, ymm2, ymm8);
    SIMD_FMA(ymm4, ymm4, ymm3, ymm9);

    SIMD_CMP(ymm6, ymm15, CMP_CMD, ymm6);   // Compare with R
    SIMD_CMP(ymm7, ymm15, CMP_CMD, ymm7);
    SIMD_CMP(ymm8, ymm15, CMP_CMD, ymm8);
    SIMD_CMP(ymm9, ymm15, CMP_CMD, ymm9);
    SIMD_AND(ymm6, ymm14, ymm6);   // Mask Color
    SIMD_AND(ymm7, ymm14, ymm7);
    SIMD_AND(ymm8, ymm14, ymm8);
    SIMD_AND(ymm9, ymm14, ymm9);

    SIMD_ADD(ymm6, ymm10, ymm10);
    SIMD_ADD(ymm7, ymm11, ymm11);
    SIMD_ADD(ymm8, ymm12, ymm12);
    SIMD_ADD(ymm9, ymm13, ymm13);

    SIMD_ADD(ymm11, ymm10, ymm10);
    SIMD_ADD(ymm12, ymm13, ymm11);
    SIMD_ADD(ymm10, ymm11, ymm10);

    SIMD_MUL(ymm10, ymm5, ymm10);       // Devide by 256
    SIMD_STORE(ymm10, result);
    *point_on_canvas = result[0] + result[1] + result[2] + result[3];
}
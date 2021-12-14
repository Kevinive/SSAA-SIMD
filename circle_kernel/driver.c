#include "circle_kernel.h"

#include "timing.h"
#include "align.h"
#include <stdio.h>
#include <math.h>

#define TEST_ROUND 10000
#define TARGET_FLOPS    4.0

void circle_kernel_naive(const double x0, const double y0, const double R, 
                    const double color, double *canvas) {
    double sum = 0.0;
    const double offset = 0.0625;
    double x = x0; 
    double y = y0;
    double dist = 0.0;
    for(int i = 0; i < 16; i++) {
        y = y0;
        for(int j = 0; j < 16; j++) {
            dist = x * x + y * y;
            if(dist < R) sum += color;
            y += offset;
        }
        x += offset;
    }
    sum /= 256;
    *canvas = sum;
}

/**
 * Main test function
 * @return
 */
int main() {
    unsigned long long sumTime = 0;
    unsigned long long startTime, endTime;
    double canvas = 0.0;
    double ref_canvas = 0.0;
    double x0 = -1.0, y0=-1.0;
    double R = 1;
    int i;
    double color = 1.0;
    char passFlag = '1';
    
    startTime = rdtsc();
    for(i = 0; i < TEST_ROUND; i++) {
        kernel_circle(x0, y0, R, color, &canvas);
    }
    endTime = rdtsc();
    sumTime += endTime - startTime;

    // printf("Ref_output:%.8lf\tSIMD_output:%.8lf\n", ref_canvas, canvas);
    circle_kernel_naive(x0, y0, R, color, &ref_canvas);
    if(fabs(canvas - ref_canvas) > 1e-5) passFlag = '0';

    printf("Rounds\tAve. Cycle\tTarget\tFLOPS\tCorrectness\n");
    printf("%d\t%.4lf\t%.4lf\t%.4lf\t%c\n", 
        TEST_ROUND, 1.0 * sumTime / TEST_ROUND, TARGET_FLOPS, 
        1.0 * TEST_ROUND * 256 / sumTime, passFlag);

    return 0;
}


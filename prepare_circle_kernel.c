#include <stdio.h>
#include "SSAA.hpp"
#include <stdio.h>

void prepare_circle_kernel(double * render_target, int width, int height, int sample_rate, 
    double r, double xc, double yc, double c, int numThread){
    int maxX = (xc + r) < width ? xc + r : width;
    int minX = (xc - r) >= 0 ? xc - r : 0;
    int maxY = (yc + r) < height ? yc + r : height;
    int minY = (yc - r) >= 0 ? yc - r : 0;
    double *target_ptr;
    int boundW = maxX - minX;
    int boundH = maxY - minY;

    int start = minY * width + minX;
    int end = maxY * height + maxX;
    int diff = maxY - minY;

    r = r * r;

    #pragma omp parallel for schedule(static, diff/numThread) num_threads(numThread)
    for (int i = minX; i < maxX; i++) {
        for (int j = minX; j < maxX; j++) {
            circle_kernel(render_target + i * width + j, j, i, xc, yc, r, c);
        }
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <algorithm>

#include "SSAA.hpp"

using namespace std;

void draw_circle(double * render_target, int width, int height, int sample_rate, 
    double r, double xc, double yc, double c) {
    int supersample_w = width * sample_rate;
    int supersample_h = height * sample_rate;

    double* supersample_target;
    posix_memalign((void**)&supersample_target, 64, 
        sizeof(double)* supersample_w * supersample_h * 4);

    for (int i = 0; i != supersample_w * supersample_h * 4; ++i)
        supersample_target[i] = 0.0;
    
    double rx = r;
    double ry = r;
    
    xc *= sample_rate;
    yc *= sample_rate;
    rx *= sample_rate;
    ry *= sample_rate;

    double maxX = xc + rx;
    double minX = xc - rx;
    double maxY = yc + ry;
    double minY = yc - ry;

    for (double y = minY; y < maxY; y++) {
        for (double x = minX; x < maxX; x++) {
            double res = ((x - xc) * (x - xc) / (rx * rx))
                + ((y - yc) * (y - yc) / (ry * ry));

            if (res <= 1.f)
                sample_point(supersample_target, 
                    supersample_w, supersample_h, x, y, c);
        } 
    }

    // Resolve
    size_t sample_total = sample_rate * sample_rate;
    for (size_t i = 0; i < width; i++){
        for (size_t j = 0; j < height; j++) {
            double r = 0, g = 0, b = 0, a = 0;
            for (size_t bi = 0; bi < sample_rate; bi++) {
                for (size_t bj = 0; bj < sample_rate; bj++) {
                    size_t pos = (i * sample_rate + bi + 
                        (j * sample_rate + bj) * supersample_w) * 4;
                    r += supersample_target[pos];
                    g += supersample_target[pos + 1];
                    b += supersample_target[pos + 2];
                    a += supersample_target[pos + 3];
                }
            }
            
            r /= sample_total; g /= sample_total;
            b /= sample_total; a /= sample_total;
            size_t idx = 4 * (i + j * width);
            render_target[idx] = r;
            render_target[idx + 1] = g;
            render_target[idx + 2] = b;
            render_target[idx + 3] = a;
        }
    }

    free(supersample_target);
}
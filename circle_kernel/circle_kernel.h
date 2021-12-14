//
// Created by kevin on 2021/10/6.
//

#ifndef CIRCLE_KERNEL_H
#define CIRCLE_KERNEL_H

#include <immintrin.h>

void kernel_circle(const double x0, const double y0, const double R, 
                    const double color, double *canvas);

#endif //CIRCLE_KERNEL_H

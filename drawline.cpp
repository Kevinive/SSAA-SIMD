#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <algorithm>

#include "SSAA.hpp"

using namespace std;

inline double fpart(double x) {
  return x - floor(x);
}

inline double rfpart(double x) {
  return 1 - fpart(x);
}

void draw_line(double * render_target, int width, int height, int sample_rate,
  double x0, double y0, double x1, double y1, double color){
  
  int supersample_w = width * sample_rate;
  int supersample_h = height * sample_rate;

  double* supersample_target;
  posix_memalign((void**)&supersample_target, 64, 
    sizeof(double)* supersample_w * supersample_h * 4);

  for (int i = 0; i != supersample_w * supersample_h * 4; ++i)
      supersample_target[i] = 0.0;

  x0 *= sample_rate;
  y0 *= sample_rate;
  x1 *= sample_rate;
  y1 *= sample_rate;

  // xiaolinwu's method
  bool steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep) {
    std::swap(x0, y0);
    std::swap(x1, y1);
  }
  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  double dx = x1 - x0;
  double dy = y1 - y0;
  double gradient = dy / dx;
  if (dx == 0.0) {
    gradient = 1.0;
  }

  // handle first endpoint
  double xend = floor(x0+0.5f);
  double yend = y0 + gradient * (xend - x0);
  double xgap = rfpart(x0 + 0.5f);
  double xpxl1 = xend; // this will be used in the main loop
  double ypxl1 = floor(yend);
  if (steep) {
    sample_point(supersample_target, supersample_w, supersample_h, 
      ypxl1, xpxl1, color * rfpart(yend) * xgap);
    sample_point(supersample_target, supersample_w, supersample_h, 
      ypxl1 + 1, xpxl1, color * fpart(yend) * xgap);
  } 
  else {
    sample_point(supersample_target, supersample_w, supersample_h, 
      xpxl1, ypxl1, color * rfpart(yend) * xgap);
    sample_point(supersample_target, supersample_w, supersample_h, 
      xpxl1, ypxl1 + 1, color * fpart(yend) * xgap);
  }
  double intery = yend + gradient; // first y-intersection for the main loop

  // handle second endpoint
  xend = floor(x1 + 0.5f);
  yend = y1 + gradient * (xend - x1);
  xgap = fpart(x1 + 0.5f);
  double xpxl2 = xend; //this will be used in the main loop
  double ypxl2 = floor(yend);
  if (steep) {
    sample_point(supersample_target, supersample_w, supersample_h, 
      ypxl2, xpxl2, color * rfpart(yend) * xgap);
    sample_point(supersample_target, supersample_w, supersample_h, 
      ypxl2 + 1, xpxl2, color * fpart(yend) * xgap);
  }
  else {
    sample_point(supersample_target, supersample_w, supersample_h, 
      xpxl2, ypxl2, color * rfpart(yend) * xgap);
    sample_point(supersample_target, supersample_w, supersample_h, 
      xpxl2, ypxl2 + 1, color * fpart(yend) * xgap);
  }

  // main loop
  if (steep) {
    for (double x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
      sample_point(supersample_target, supersample_w, supersample_h, 
        floor(intery), x, color * rfpart(intery));

      sample_point(supersample_target, supersample_w, supersample_h, 
        floor(intery) + 1, x, color * fpart(intery));

      intery = intery + gradient;
    }
  }
  else {
    for (double x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
      sample_point(supersample_target, supersample_w, supersample_h, 
        x, floor(intery), color * rfpart(intery));

      sample_point(supersample_target, supersample_w, supersample_h, 
        x, floor(intery) + 1, color * fpart(intery));

      intery = intery + gradient;
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
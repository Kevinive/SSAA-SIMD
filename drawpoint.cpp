#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <algorithm>

using namespace std;

void fill_pixel(double* supersample_target, 
  int width, int height, int sample_rate,
  double x, double y, double c) {
    
  // fill in the nearest pixel
  int sx = (int)floor(x);
  int sy = (int)floor(y);

  // check bounds
  if (sx < 0 || sx >= width) return;
  if (sy < 0 || sy >= height) return;

  for (size_t i = 0; i < sample_rate; i++) {
    for (size_t j = 0; j < sample_rate; j++) {
      size_t pos = (sx * sample_rate + i +
        (sy * sample_rate + j) * sample_rate * width) * 4;

      // fill sample
      supersample_target[pos] = c;
      supersample_target[pos + 1] = c;
      supersample_target[pos + 2] = c;
      supersample_target[pos + 3] = 1.0;
    }
  }
}

void sample_point(double* supersample_target, 
  int supersample_w, int supersample_h, 
  double x, double y, double c) {

  // fill in the nearest pixel
  int sx = (int)floor(x);
  int sy = (int)floor(y);

  // check bounds
  if (sx < 0 || sx >= supersample_w) return;
  if (sy < 0 || sy >= supersample_w) return;

  size_t pos = 4 * (sx + sy * supersample_w);

  supersample_target[pos] = c;
  supersample_target[pos + 1] = c;
  supersample_target[pos + 2] = c;
  supersample_target[pos + 3] = 1.0;
}
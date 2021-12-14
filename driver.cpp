#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <utility> 
#include <algorithm>
#include <iostream>

#include "lodepng.h"
#include "SSAA.hpp"

#define _POSIX_C_SOURCE 200809L

unsigned long long rdtsc()
{
  unsigned long long int x;
  unsigned a, d;

  __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));

  return ((unsigned long long)a) | (((unsigned long long)d) << 32);
}

void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height) {
  //Encode the image
  unsigned error = lodepng::encode(filename, image, width, height);

  //if there's an error, display it
  if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

void drawlineDriver (int runs, int N, int sample_rate, int width, int height){
  double *true_render_target;
  posix_memalign((void**)&true_render_target, 64, sizeof(double)*width*height*4);
  double *test_render_target;
  posix_memalign((void**)&test_render_target, 64, sizeof(double)*width*height);

  for (int i = 0; i != width*height*4; ++i)
    true_render_target[i] = 0.0;
  
  unsigned long long t0, t1;

  unsigned long long true_sum = 0;
  for (int r = 0; r != runs/10; ++r){
    for (int i = 0; i != N; i++){
      double x0 = 0.0;
      double y0 = 0.0;
      double x1 = width - 1.0;
      double y1 = height / 2;
      double color = 1.0;

      // double x0 = ((double) rand())/ ((double) RAND_MAX) * ((double) width - 1.0);
      // double x1 = ((double) rand())/ ((double) RAND_MAX) * ((double) width - 1.0);
      // double y0 = ((double) rand())/ ((double) RAND_MAX) * ((double) height - 1.0);
      // double y1 = ((double) rand())/ ((double) RAND_MAX) * ((double) height - 1.0);
      // double color = ((double) rand())/ ((double) RAND_MAX);

      t0 = rdtsc();
      draw_line(true_render_target, width, height, sample_rate, x0, y0, x1, y1, color);
      t1 = rdtsc();

      true_sum += (t1 - t0); 
    } 
  }
  // printf("baseline: %.3lf\n",true_sum/(1.0*runs * N));
  // printf("\n====================\n");
  // for (int i = 0; i != width; i++){
  //   for (int j = 0; j != height; j++){	
  //     printf(" %.1lf ", true_render_target[4*(j + i * height)]);
  //   }
  //   printf("\n");
  // }
  // printf("\n====================\n");

  std::vector<unsigned char> true_image;
  true_image.resize(width * height * 4);
  
  for (int i = 0; i != width; i++){
    for (int j = 0; j != height; j++){	
      char val = 255 * true_render_target[4*(i + j * width)] * 8.f;
      true_image[4 * width * j + 4 * i + 0] = val;
      true_image[4 * width * j + 4 * i + 1] = val;
      true_image[4 * width * j + 4 * i + 2] = val;
      true_image[4 * width * j + 4 * i + 3] = 255;
    }
  }
  std::string true_filename = "true_line" + std::to_string(width) 
    + "x" + std::to_string(height) + ".png";
  encodeOneStep(true_filename.c_str(), true_image, width, height);

  for (int i = 0; i <= width*height; ++i)
    test_render_target[i] = 0.0;

  unsigned long long test_sum = 0;
  unsigned long long kernelSum = 0;
  unsigned long long threadSum = 0;
  int line_pixels = 0;
  int k = 1;
  for (k = 1; k <= 16; k *= 2){
    for (int r = 0; r != runs; ++r){
      t0 = rdtsc();
      #pragma omp parallel for num_threads(k) reduction(+:kernelSum)
      for (int i = 0; i < N; i++){
        double x0 = 0.0;
        double y0 = 0.0;
        double x1 = width - 1.0;
        double y1 = height / 2;
        double color = 1.0;

        // double x0 = ((double) rand())/ ((double) RAND_MAX) * ((double) width - 1.0);
        // double x1 = ((double) rand())/ ((double) RAND_MAX) * ((double) width - 1.0);
        // double y0 = ((double) rand())/ ((double) RAND_MAX) * ((double) height - 1.0);
        // double y1 = ((double) rand())/ ((double) RAND_MAX) * ((double) height - 1.0);
        // double color = ((double) rand())/ ((double) RAND_MAX);

        prepare_line_kernel(test_render_target, width, height, sample_rate, 
          x0, y0, x1, y1, color, line_pixels, k,kernelSum);
      } 
      t1 = rdtsc();
      test_sum += (t1 - t0); 
      threadSum += (t1 - t0);
    }

    printf("%d, %.3lf, %.3lf\n", k, ((double) (threadSum  / (1.0*runs))),
      ((double) (kernelSum  / (1.0*runs))));
    threadSum = 0;
    kernelSum = 0;
  }

  // printf("\n====================\n");
  // for (int i = 0; i != width; i++){
  //   for (int j = 0; j != height; j++){	
  //     printf(" %.1lf ", test_render_target[j + i * height]);
  //   }
  //   printf("\n");
  // }
  // printf("\n====================\n");
  std::vector<unsigned char> test_image;
  test_image.resize(width * height * 4);
  
  for (int i = 0; i != width; i++){
    for (int j = 0; j != height; j++){	
      char val = 255 * test_render_target[i + j * width] * 8.f;
      test_image[4 * width * j + 4 * i + 0] = val;
      test_image[4 * width * j + 4 * i + 1] = val;
      test_image[4 * width * j + 4 * i + 2] = val;
      test_image[4 * width * j + 4 * i + 3] = 255;
    }
  }

  std::string test_filename = "test_line" + std::to_string(width) 
    + "x" + std::to_string(height) + ".png";
  encodeOneStep(test_filename.c_str(), test_image, width, height);

  int flops = (108*4+93+20)*line_pixels;
  double fpc = flops/(test_sum/(1.0*runs * N * 5.0));
  double ofPeak = fpc/4.0;

  printf("%d\t%d\t%.3lf\t%.3lf\t%d\t%.3lf\t%.3lf\t", N, width, 
    true_sum/(1.0*runs * N), test_sum/(5.0*runs), flops, fpc,ofPeak);

  printf("\n");

  free(true_render_target);
  free(test_render_target);
}


void drawcircleDriver (int runs, int N, int sample_rate, int width, int height){
  double *true_render_target;
  double *test_render_target;

  posix_memalign((void**)&true_render_target, 64, sizeof(double)*width*height*4);
  posix_memalign((void**)&test_render_target, 64, sizeof(double)*width*height*4);
  
  unsigned long long t0, t1;
  
  for (int i = 0; i != width*height*4; ++i)
    true_render_target[i] = 0.0;

  unsigned long long true_sum = 0;
  for (int r = 0; r != runs; ++r){
    for (int i = 0; i != N; i++){
      double r = width - 1.0;
      double xc = 0.0;
      double yc = 0.0;
      double color = 1.0;

      // double r = ((double) rand())/ ((double) RAND_MAX) * ((double) width - 1.0)*0.5;
      // double xc = ((double) rand())/ ((double) RAND_MAX) * ((double) height - 1.0);
      // double yc = ((double) rand())/ ((double) RAND_MAX) * ((double) height - 1.0);
      // double color = ((double) rand())/ ((double) RAND_MAX);
      t0 = rdtsc();
      draw_circle(true_render_target, width, height, sample_rate, r, xc, yc, color);
      t1 = rdtsc();

      true_sum += (t1 - t0); 
    } 
  }

  // printf("\n====================\n");
  // for (int i = 0; i != width; i++){
  //   for (int j = 0; j != height; j++){	
  //     printf(" %.1lf ", true_render_target[4*(j + i * height)]);
  //   }
  //   printf("\n");
  // }
  // printf("\n====================\n");

  std::vector<unsigned char> true_image;
  true_image.resize(width * height * 4);
  
  for (int i = 0; i != width; i++){
    for (int j = 0; j != height; j++){	
      char val = 255 * true_render_target[4*(i + j * width)];
      true_image[4 * width * j + 4 * i + 0] = val;
      true_image[4 * width * j + 4 * i + 1] = val;
      true_image[4 * width * j + 4 * i + 2] = val;
      true_image[4 * width * j + 4 * i + 3] = 255;
    }
  }
  std::string true_filename = "true_circle" + std::to_string(width) 
    + "x" + std::to_string(height) + ".png";
  encodeOneStep(true_filename.c_str(), true_image, width, height);

  for (int i = 0; i <= width*height; ++i)
    test_render_target[i] = 0.0;

  unsigned long long test_sum = 0;
  unsigned long long threadSum = 0;
  for (int k = 1; k <= 16; k *= 2){
    for (int r = 0; r != runs; ++r){
      t0 = rdtsc();
      #pragma omp parallel for num_threads(k)
      for (int i = 0; i < N; i++){
        double r = width - 1.0;
        double xc = 0.0;
        double yc = 0.0;
        double color = 1.0;

        // double r = ((double) rand())/ ((double) RAND_MAX) * ((double) width - 1.0)*0.5;
        // double xc = ((double) rand())/ ((double) RAND_MAX) * ((double) height - 1.0);
        // double yc = ((double) rand())/ ((double) RAND_MAX) * ((double) height - 1.0);
        // double color = ((double) rand())/ ((double) RAND_MAX);
        
        prepare_circle_kernel(test_render_target, width, height, sample_rate, 
          r, xc, yc, color, k);
      }
      t1 = rdtsc();
      test_sum += (t1 - t0); 
      threadSum += (t1 - t0);
    }
    printf("%d,%.3lf\n", k, ((double) (threadSum  / (1.0*runs))));
    threadSum = 0;
  }

  // printf("\n====================\n");
  // for (int i = 0; i != width; i++){
  //   for (int j = 0; j != height; j++){	
  //     printf(" %.1lf ", test_render_target[i + j * width]);
  //   }
  //   printf("\n");
  // }
  // printf("\n====================\n");

  std::vector<unsigned char> test_image;
  test_image.resize(width * height * 4);
  
  for (int i = 0; i != width; i++){
    for (int j = 0; j != height; j++){	
      char val = 255 * test_render_target[i + j * width];
      test_image[4 * width * j + 4 * i + 0] = val;
      test_image[4 * width * j + 4 * i + 1] = val;
      test_image[4 * width * j + 4 * i + 2] = val;
      test_image[4 * width * j + 4 * i + 3] = 255;
    }
  }

  std::string test_filename = "test_circle" + std::to_string(width) 
    + "x" + std::to_string(height) + ".png";
  encodeOneStep(test_filename.c_str(), test_image, width, height);

  printf("%d\t%d\t%.3lf\t%.3lf\t", N, width, true_sum/(1.0*runs * N), test_sum/(5.0*runs));
  
  printf("\n");

  free(true_render_target);
  free(test_render_target);
}

int main(int argc, char** argv){
  
  int width = 256;
  int height = 256;

  int runs = 1;
  int N = 4;  //size of objects
  int sample_rate = 16;

  if (argc < 3){
    printf("transpose.x <<Number of Runs>> <<Number of Objects>> <<Sample Rate>>\n");
    exit(0);
  }  
  else{
    runs = atoi(argv[1]);
    N = atoi(argv[2]);
    sample_rate = atoi(argv[3]);
  }

  printf("\n################## Draw Line: ###################\n");
  for (int i = 8; i <= 1024; i *= 2){
    drawlineDriver(runs, N, sample_rate, i, i);
  }

  // drawlineDriver(runs, N, sample_rate, 32, 32);
  printf("Done\n");
  printf("\n#################################################\n");
  
  printf("\n################## Draw Circle: ###################\n");
  for (int i = 8; i <= 1024; i *= 2){
    drawcircleDriver(runs, N, sample_rate, i, i);
  }

  // drawcircleDriver(runs, N, sample_rate, 32, 32);
  printf("Done\n");
  printf("\n#################################################\n");

  // draw custom image
  height = 1024;
  width = 1024;
  sample_rate = 16;

  double *render_target;
  posix_memalign((void**)&render_target, 64, sizeof(double)*width*height);
  memset(render_target, 0, sizeof(double)*width*height);

  double r = 256.0;
  double xc = 511.0;
  double yc = 511.0;
  double color = 1.0;

  prepare_circle_kernel(render_target, width, height, sample_rate, 
          r, xc, yc, color, 4);

  double x0 = 0.0;
  double y0 = 0.0;
  double x1 = 1023.0;
  double y1 = 1023.0;
  color = 1.0;
  unsigned long long kernelSum = 0;
  int line_pixels = 0;

  prepare_line_kernel(render_target, width, height, sample_rate, 
    x0, y0, x1, y1, color, line_pixels, 4, kernelSum);

  x0 = 0.0;
  y0 = 1023.0;
  x1 = 1023.0;
  y1 = 0.0;
  color = 1.0;

  prepare_line_kernel(render_target, width, height, sample_rate, 
    x0, y0, x1, y1, color, line_pixels, 4, kernelSum);

  std::vector<unsigned char> image;
  image.resize(width * height * 4);
  
  for (int i = 0; i != height; i++){
    for (int j = 0; j != width; j++){	
      char val = 255 * render_target[i * width + j];
      image[4 * width * i + 4 * j + 0] = val;
      image[4 * width * i + 4 * j + 1] = val;
      image[4 * width * i + 4 * j + 2] = val;
      image[4 * width * i + 4 * j + 3] = 255;
    }
  }
  std::string filename = "demo.png";
  encodeOneStep(filename.c_str(), image, width, height);

  return 0;
}

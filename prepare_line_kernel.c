#include <vector>
#include <utility> 
#include <algorithm>

#include "SSAA.hpp"

#define num_input 5
static unsigned long long rdtsc()
{
  unsigned long long int x;
  unsigned a, d;

  __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));

  return ((unsigned long long)a) | (((unsigned long long)d) << 32);
}
void prepare_line_kernel(double * render_target, int width, int height, int sample_rate,
    double x0, double y0, double x1, double y1, double color, int& line_pixels, int numThread, unsigned long long &cycles){
    int supersample_w = width * sample_rate;
    int supersample_h = height * sample_rate;

    x0 *= sample_rate;
    y0 *= sample_rate;
    x1 *= sample_rate;
    y1 *= sample_rate;

    double dx = x1 - x0;
    double dy = y1 - y0;
    double gradient = dy / dx;
    double b = y0 - gradient * x0;

    int sz = supersample_w / sample_rate;
    std::vector< std::pair<double,double> > intersects (sz); 

    // #pragma omp declare reduction (push : std::vector<std::pair<double,double>>
    //     : intersects.push_back(tmp)
    
    // printf("Number of thread: %d\n", numThread);

    // #pragma omp parallel for num_threads(numThread)
    for (int i = 0.0; i < sz; i++) {
        
        double i_x = 0.0;
        double i_y = 0.0;

        double px = i * sample_rate;
        double py = px * gradient + b;
        if (px < std::min(x0, x1) 
            || px >= std::max(x0, x1) 
            || py < std::min(y0, y1) 
            || py >= std::max(y0, y1))
            continue;

        std::pair<double,double> tmp = {px, py};
        intersects[i] = tmp;
    }

    // std::sort(intersects.begin(), intersects.end()); 
    line_pixels = intersects.size();
    unsigned long long t0, t1;
    // #pragma omp parallel for num_threads(numThread)
    for (int i = 0; i < intersects.size(); i+=num_input){
        double res[num_input] = {0};
        alignas(32) double tempStore[num_input*4] = {0};
        double x[num_input * 2] = {0};
        double y[num_input * 2] = {0};
        double sx[num_input * 2] = {0};
        double sy[num_input * 2] = {0};

        for (int j = 0; j < num_input; j++){
            x[2 * j] = (i + j < intersects.size()) ? 
                (int)intersects[i + j].first % sample_rate : 0.0;
            x[2 * j + 1] = (i + j + 1 < intersects.size()) ? 
                (int)intersects[i + j + 1].first % sample_rate : 0.0;
            y[2 * j] = (i + j < intersects.size()) ? 
                (int)intersects[i + j].second % sample_rate : 0.0;
            y[2 * j + 1] = (i + j + 1 < intersects.size()) ? 
                (int)intersects[i + j + 1].second % sample_rate : 0.0;

            sx[2 * j] = (i + j < intersects.size()) ? 
                (int)intersects[i + j].first / sample_rate : 0.0;
            sx[2 * j + 1] = (i + j + 1 < intersects.size()) ? 
                (int)intersects[i + j + 1].first / sample_rate : 0.0;
            sy[2 * j] = (i + j < intersects.size()) ? 
                (int)intersects[i + j].second / sample_rate : 0.0;
            sy[2 * j + 1] = (i + j + 1 < intersects.size()) ? 
                (int)intersects[i + j + 1].second / sample_rate : 0.0;
        }

        // draw line kernel here
        t0 = rdtsc();
        line_kernel(tempStore, x, y, color, gradient, num_input);
        
        for (int i = 0; i < 4; ++i){
            res[0]+=tempStore[0+i];
            res[1]+=tempStore[4+i];
            res[2]+=tempStore[8+i];
            res[3]+=tempStore[12+i];
            res[4]+=tempStore[16+i];
        }
        
        for (int i = 0; i < num_input; i++){
            int tx = sx[i * 2];
            int ty = sy[i * 2];
            size_t idx = tx + ty * width;
            //printf("res %lf, %d,%d,%d\n",res[i],idx,tx,ty);
            render_target[idx] = res[i];
        }
        t1 = rdtsc();
        cycles += (t1 - t0);
    }
}
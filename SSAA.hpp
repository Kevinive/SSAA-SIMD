
void fill_pixel(double* supersample_target, 
  int width, int height, int sample_rate,
  double x, double y, double c);

void sample_point(double* supersample_target, 
  int supersample_w, int supersample_h, 
  double x, double y, double c);

void draw_line(double * render_target, int width, int height, int sample_rate,
  double x0, double y0, double x1, double y1, double color);

void draw_circle(double * render_target, int width, int height, int sample_rate, 
    double r, double xc, double yc, double c);

void prepare_line_kernel(double * render_target, int width, int height, int sample_rate,
  double x0, double y0, double x1, double y1, double color, int& line_pixels, int numThread,
  unsigned long long &cycles);

void line_kernel(double *target, double *x, double *y,
    double color, double gradient, int n);

void prepare_circle_kernel(double * render_target, int width, int height, int sample_rate, 
    double r, double xc, double yc, double c, int numThread);

void circle_kernel(double* point_on_canvas, double x0, double y0, 
    double xc, double yc, double r, double color);
  
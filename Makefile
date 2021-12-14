CC = g++
CFLAGS = -O1 -std=c++11 -mavx -mfma -fopenmp

OBJS = line_kernel.o circle_kernel.o drawpoint.o drawline.o drawcircle.o prepare_line_kernel.o prepare_circle_kernel.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

compile: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) driver.cpp align.cpp lodepng.cpp -o SSAA.x -march=native

run:
	./SSAA.x
	./SSAA.x 100 4 16

clean:
	rm -f *.x *~ *.o
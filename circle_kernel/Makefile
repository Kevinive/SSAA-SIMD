CC = gcc
CFLAGS = -O1 -std=c99 -mavx -mavx2 -mfma

OBJS = circle_kernel.o 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

compile: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) driver.c timing.c align.c -o circle_test.x -march=native

run:
	./circle_test.x

clean:
	rm -f *.x *~ *.o
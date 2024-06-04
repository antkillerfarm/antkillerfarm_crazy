CC=g++
CFLAGS=-g -Wall -Wfatal-errors -std=c++11 -lOpenCL -lsvml -lirng -lintlc -limf -L/opt/intel/oneapi/2024.1/lib -I/opt/intel/oneapi/2024.1/include/sycl

all: vadd

vadd: vadd.c
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm -f *.o vadd

# build hiOpenwrt executable when user executes "make"
hiOpenwrt: hiOpenwrt.o
	$(CC) $(LDFLAGS) hiOpenwrt.o -o hiOpenwrt

hiOpenwrt.o: hiOpenwrt.c
	$(CC) $(CFLAGS) -c hiOpenwrt.c
#remove object files and executable when user executes "make clean"
clean:
	rm *.o hiOpenwrt


all: fs.exe

fs.exe: flash_fs.o flash_drv.o main.o crc16.o
	gcc -o $@ $^

%.o: %.c
	gcc -g -o $@ -c $^

clean:
	rm *.exe *.o *.dat



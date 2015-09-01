default:
	@echo make link "----the demo of dynamic link"
	@echo make load "----the demo of dynamic load"

link:main_link

load:main_load

main_link:main_link.c libhello.so
	gcc -o main_link main_link.c -L. -lhello

main_load:main_load.c libhello.so
	gcc -o main_load main_load.c -ldl

libhello.so:hello.c
	gcc hello.c -fPIC -shared -o libhello.so

clean:
	rm libhello.so main_link main_load

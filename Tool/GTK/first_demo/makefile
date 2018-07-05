CFLAGS = `pkg-config --cflags --libs gtk+-3.0 gmodule-export-2.0`

first_demo:main.c first_demo.glade
	gcc main.c -o first_demo $(CFLAGS)
clean:
	rm first-demo

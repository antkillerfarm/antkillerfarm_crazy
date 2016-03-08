CFLAGS = `pkg-config --cflags gstreamer-1.0 gtk+-3.0 gmodule-export-2.0 libxml-2.0`
LDFLAGS = `pkg-config --libs gstreamer-1.0 gtk+-3.0 gmodule-export-2.0 libxml-2.0`

hello: helloworld.o
	gcc $^ -o $@ $(LDFLAGS)

%.o: %.c
	gcc -g $(CFLAGS) -o $@ -c $^

clean:
	rm helloworld *.o

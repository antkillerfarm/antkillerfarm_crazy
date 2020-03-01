SRCS = $(wildcard ./*.ts)
OBJS = $(patsubst %.ts, ./%.js, $(SRCS))

.PHONY: all

all: $(OBJS)

%.js: %.ts
	tsc $^

clean:
	rm *.js

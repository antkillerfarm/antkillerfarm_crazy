SRCS = $(wildcard ./*.tex)
OBJS = $(patsubst %.tex, ./%.pdf,$(SRCS)) 

.PHONY: all

all: $(OBJS)

%.pdf: %.tex
	xelatex --enable-write18 --shell-escape $^

clean:
	rm *.aux *.log

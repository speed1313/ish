SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
CC=gcc

ish: $(SRCS) main.h stack.h
		$(CC)  -o ish $(SRCS)

test: ish
		./test.sh

clean:
		rm -f ish *.o *~ tmp*

.PHONY: test clean
CFLAGS=-std=c11 -fno-common -Wall -Wno-switch

SRCS=$(wildcard *.c)
EXES=$(SRCS:.c=)

.PHONY: all
all: $(EXES)

clean:
	rm -f $(EXES)

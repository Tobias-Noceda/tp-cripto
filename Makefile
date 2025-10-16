include Makefile.inc

ifneq ($(DEBUG), 0)
CFLAGS += -g -DDEVELOPMENT
else
CFLAGS += -O2
endif

ROOT = ./src

SRCS = $(wildcard $(ROOT)/readers/*.c) $(wildcard $(ROOT)/writers/*.c) $(wildcard $(ROOT)/bmp/*.c) $(ROOT)/stego.c
HDRS = $(ROOT)/include

OBJS = $(SRCS:.c=.o)

EXEC = bin/stego

all: $(EXEC)

$(EXEC):
	@mkdir -p bin
	$(CC) $(SRCS) $(CFLAGS) -I$(HDRS) -o $@

clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean

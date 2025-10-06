ifneq ($(DEBUG), 0)
CFLAGS += -g -DDEVELOPMENT
else
CFLAGS += -O2
endif

ROOT = ./src

SRCS = $(wildcard $(ROOT)/readers/*.c) $(wildcard $(ROOT)/writers/*.c) $(ROOT)/stego.c
HDRS = $(ROOT)/include

OBJS = $(SRCS:.c=.o)

EXEC = bin/stego

all: $(EXEC)

$(EXEC):
	@mkdir -p bin
	$(CC) $(CFLAGS) -I$(HDRS) -o $@ $(SRCS) -lm

clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean

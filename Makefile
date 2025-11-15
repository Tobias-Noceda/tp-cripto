include Makefile.inc

ifneq ($(DEBUG), 0)
CFLAGS += -g -DDEVELOPMENT
else
CFLAGS += -O2
endif

ifneq ($(LOGS), 0)
CFLAGS += -DVERBOSE
endif

ROOT = ./src

SRCS = $(wildcard $(ROOT)/crypto/*.c) $(wildcard $(ROOT)/readers/*.c) $(wildcard $(ROOT)/writers/*.c) $(wildcard $(ROOT)/bmp/*.c) $(wildcard $(ROOT)/*.c)
HDRS = $(ROOT)/include

OBJS = $(SRCS:.c=.o)

EXEC = ./bin/stego

all: $(EXEC)

$(EXEC):
	@mkdir -p ./bin
	$(CC) $(SRCS) $(CFLAGS) -I$(HDRS) -o $@

check: all
	@echo "Running tests..."
	@./test.sh
	@echo "All tests done!"

clean:
	rm -rf $(OBJS) $(EXEC) ./bin ./runs

install:
	chmod u+x stegobmp *.sh

.PHONY: install all clean

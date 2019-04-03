SHELL= /bin/sh
CC=gcc
CFLAGS= -shared -fPIC
LDFLAGS= -ldl

INCLUDEDIR=include
SOURCEDIR=src

DEPS= $(shell echo include/*.h)
SRCS= $(shell echo src/*.c)

TARGET= libTTThwart.so

all: $(TARGET)

$(TARGET): $(SRCS) $(DEPS)
	$(CC) $(CFLAGS) $(SRCS) -I$(INCLUDEDIR) -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f $(TARGET)
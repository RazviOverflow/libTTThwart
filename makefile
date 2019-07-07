SHELL= /bin/sh
CC=gcc
CFLAGS= -shared -fPIC -Wall -Wextra -fstack-protector-all -O2 -lpthread -ldl

INCLUDEDIR=include
SOURCEDIR=src

DEPS= $(shell echo include/*.h)
SRCS= $(shell echo src/*.c)

TARGET= libTTThwart.so

library:
	$(CC) $(CFLAGS) $(SRCS) -I$(INCLUDEDIR) -o $(TARGET) 

debug: 
	$(CC) $(CFLAGS) $(SRCS) -I$(INCLUDEDIR) -o $(TARGET) -D DEBUG

.PHONY: clean

clean:
	rm -f $(TARGET)
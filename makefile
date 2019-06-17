SHELL= /bin/sh
CC=gcc
CFLAGS= -shared -fPIC -Wall -Wextra
LDFLAGS= -ldl

INCLUDEDIR=include
SOURCEDIR=src

DEPS= $(shell echo include/*.h)
SRCS= $(shell echo src/*.c)

TARGET= libTTThwart.so

all: $(TARGET)

$(TARGET): $(SRCS) $(DEPS)
	$(CC) $(CFLAGS) $(SRCS) -I$(INCLUDEDIR) -o $@ $(LDFLAGS)

ALLINONESRCS= ALLinONE.c src/zlog.c

allinone: 
	gcc -shared -fpic -Wall -Wextra $(ALLINONESRCS) -I$(INCLUDEDIR) -o ALLinONE.so -ldl -lpthread 

.PHONY: clean

clean:
	rm -f $(TARGET)
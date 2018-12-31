CC=gcc
LINK=gcc

GPIO_LIBS=-lpigpio

GTKINCLUDES=`pkg-config --cflags gtk+-3.0`
GTKLIBS=`pkg-config --libs gtk+-3.0`

OPTIONS=-g -Wno-deprecated-declarations -O3

LIBS=-lrt -lm -lpthread $(GTKLIBS) $(GPIO_LIBS) 
INCLUDES=$(GTKINCLUDES)

COMPILE=$(CC) $(OPTIONS) $(INCLUDES)

.c.o:
	$(COMPILE) -c -o $@ $<

PROGRAM=monitor

SOURCES= \
vswr-monitor.c \
meter.c \

HEADERS= \
meter.h \

OBJS= \
vswr-monitor.o \
meter.o \

$(PROGRAM):  $(OBJS) 
	$(LINK) -o $(PROGRAM) $(OBJS) $(LIBS)

all: prebuild  $(PROGRAM) $(HEADERS) 


clean:
	-rm -f *.o
	-rm -f $(PROGRAM)
CC=gcc
LINK=gcc

OPTIONS=-g 

LIBS=-lrt -lm -lpigpio -lpthread 

COMPILE=$(CC) $(OPTIONS) $(INCLUDES)

PROGRAM=vswr-monitor

SOURCES= \
vswr.c 

HEADERS= 


OBJS= \
vswr.o 

all: prebuild $(PROGRAM) $(HEADERS) $(SOURCES) 

prebuild:
	rm -f version.o

$(PROGRAM): $(OBJS) 
	$(LINK) -o $(PROGRAM) $(OBJS) $(LIBS)

.c.o:
	$(COMPILE) -c -o $@ $<


clean:
	-rm -f *.o
	-rm -f $(PROGRAM)


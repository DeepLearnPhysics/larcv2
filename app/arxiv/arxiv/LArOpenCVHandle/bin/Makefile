CC=g++
CFLAGS=-g
INCFLAGS =-I.
INCFLAGS+=-I$(shell larcv-config --incdir)
INCFLAGS+=-I$(OPENCV_INCDIR)
INCFLAGS+=$(shell root-config --cflags)


LDFLAGS=-L.
LDFLAGS+=$(shell larcv-config --libs)
LDFLAGS+=$(shell root-config --libs)


SOURCES=run.cxx
OBJECTS=$(SOURCES:.cxx=.o)
EXECUTABLE=run

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

clean:
	rm *.o run

CC=gcc
CCFLAGS=-Wall -g -O0
LDFLAGS=
SOURCES=perf.c ac-backend.c matcher-ac.c node-table.c main.c
OBJECTS=$(SOURCES:.c=.o)
TARGET=ac-test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CCFLAGS) -c $<

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f *.o $(TARGET)

clean-all: clean
	rm -rf *~ *#

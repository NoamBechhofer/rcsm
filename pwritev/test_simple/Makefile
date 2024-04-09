CC=gcc
CFLAGS=-Wall -Wpedantic -g

TARGET=pwritev_test

SOURCES=pwritev_test.c
OBJECTS=$(SOURCES:.c=.o)

all: $(TARGET)

$(TARGER): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean

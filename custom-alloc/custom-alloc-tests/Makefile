CC=gcc
CFLAGS=-I/usr/include/CUnit -Wall
LDFLAGS=-lcunit
SOURCES=test_main.c
OBJECTS=$(SOURCES:.c=.o)
TARGET=test_runner

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
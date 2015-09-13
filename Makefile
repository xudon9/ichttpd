CC=gcc

CFLAGS=-c -O2 -Wall

LDFLAGS=-Wall -O2

SOURCES=main.c ichttpd.c utils.c
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=ichttpd

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@echo "Removing..."
	rm -f $(OBJECTS) $(EXECUTABLE)

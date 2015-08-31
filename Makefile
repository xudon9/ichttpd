#编译器
CC=gcc

#编译选项
CFLAGS=-c -O2 -Wall

#链接选项
LDFLAGS=-Wall -O2

#将所有源文件添加到 SOURCES
SOURCES=main.c ichttpd.c utils.c
OBJECTS=$(SOURCES:.c=.o)

#可执行文件名
EXECUTABLE=ichttpd

all: $(SOURCES) $(EXECUTABLE)
	cp $(EXECUTABLE) ./www

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@echo "Removing..."
	rm -f $(OBJECTS) $(EXECUTABLE)

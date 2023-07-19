CC = gcc 
CFLAGS += -Wall -O2 -pthread
OBJS = main.o
TARGET = main

all = $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o
	rm $(TARGET)
	find -type f ! -name "*.c", ! -name "Makefile" ! -name "hello_world" -exec rm {} \;

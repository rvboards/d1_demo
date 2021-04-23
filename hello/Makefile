CC=gcc
TARGET=hello
OBJS=hello.o
CFLAGS=-O2

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)


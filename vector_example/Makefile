CC=riscv64-unknown-linux-gnu-gcc
TARGET=vector_example
OBJS=vector_example.o
CFLAGS=-O2 -march=rv64gcvxtheadc -mabi=lp64d -mtune=c906

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -static -o $(TARGET) $(OBJS)

qemu_run:$(TARGET)
	qemu-riscv64 -cpu c906fdv ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)


CC = gcc
CFLAGS = -g

TARGET = disassem
DEPS = PA1.o

$(TARGET): $(DEPS)
	$(CC) $(CFLAGS) $(DEPS) -o $(TARGET)

PA1.o: PA1.c
	$(CC) $(CFLAGS) -c PA1.c

test: $(TARGET)
	./$(TARGET) test.obj

clean:
	rm -f $(DEPS) $(TARGET)
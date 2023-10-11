CC=gcc
CFLAGS=-Wall
EXECUTABLE=resolver
resolver: main.c
	$(CC) $(CFLAGS) main.c -o $(EXECUTABLE)
clean:
	rm -rf *.o *.gch $(EXECUTABLE)
leaks:
	valgrind --leak-check=yes ./$(EXECUTABLE)

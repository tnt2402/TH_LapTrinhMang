CC=gcc
CFLAGS=-Wall
EXECUTABLE=hw3
hw3: linkedlist.o main.c
	$(CC) $(CFLAGS) linkedlist.o main.c -o $(EXECUTABLE)
linkedlist.o: linkedlist.h 
clean:
	rm -rf *.o *.gch $(EXECUTABLE)
leaks:
	valgrind --leak-check=yes ./$(EXECUTABLE)

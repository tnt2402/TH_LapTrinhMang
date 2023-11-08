CC=gcc
CFLAGS=-Wall
EXECUTABLE_server=server
EXECUTABLE_client=client


all: server client

server: linkedlist.o server.c
	$(CC) $(CFLAGS) linkedlist.o server.c -o $(EXECUTABLE_server)
linkedlist.o: linkedlist.h 
	
client: client.c
	$(CC) $(CFLAGS) client.c -o $(EXECUTABLE_client)

clean:
	rm -f server client
leaks:
	valgrind --leak-check=yes ./$(EXECUTABLE)

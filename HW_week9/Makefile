CC=gcc
CFLAGS=-Wall
EXECUTABLE_server=server
EXECUTABLE_client=client

all: server client

server: server.c
	$(CC) $(CFLAGS) server.c -o $(EXECUTABLE_server)
	
client: client.c
	$(CC) $(CFLAGS) client.c -o $(EXECUTABLE_client)
clean:
	rm -f server client
leaks:
	valgrind --leak-check=yes ./$(EXECUTABLE)

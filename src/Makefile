CC=gcc
CFLAGS=-I.

all: server client

server: ./server/server.c
	$(CC) -o ./server/server ./server/server.c $(CFLAGS)

client: ./client/client.c
	$(CC) -o ./client/client ./client/client.c $(CFLAGS)

clean:
	rm -f ./server/server ./client/client

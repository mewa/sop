CFLAGS = -Wall -Iserver_src -Iclient_src -Ishared_src
SERVER_SRC=$(wildcard ./server_src/*.c) $(wildcard ./shared_src/*.c)
CLIENT_SRC=$(wildcard ./client_src/*.c) $(wildcard ./shared_src/*.c)

all: server client

server: $(SERVER_SRC)
	gcc -o $@ $^ $(CFLAGS)

client: $(CLIENT_SRC)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm server client *~

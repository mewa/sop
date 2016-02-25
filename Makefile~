CFLAGS = -Wall
SERVER_SRC=$(wildcard ./server_src/*.c) $(wildcard ./shared_src/*.c)
CLIENT_SRC=$(wildcard ./client_src/*.c) $(wildcard ./shared_src/*.c)

server: $(SERVER_SRC)
	gcc -o $@ $^ $(CFLAGS)

client: $(CLIENT_SRC)
	gcc -o $@ $^ $(CFLAGS)

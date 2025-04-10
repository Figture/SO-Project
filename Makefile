CC = gcc
CFLAGS = -Wall -g -Iinclude $(shell pkg-config --cflags glib-2.0)
LDFLAGS = $(shell pkg-config --libs glib-2.0)

all: folders dserver dclient

dserver: bin/dserver

dclient: bin/dclient

folders:
	@mkdir -p src include obj bin tmp fifos

bin/dserver: src/dserver.c obj/services.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

bin/dclient: src/dclient.c obj/services.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

obj/%.o: src/%.c  
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f obj/* tmp/* bin/* fifos/*


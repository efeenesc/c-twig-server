CC=clang
CFLAGS=-O3 -std=c99
SRCFILES= main.c server.c http.c \
utils/hashmap.c utils/string_util.c \
types/key_value_pair.c types/http_enum.c

ifndef VERBOSE
.SILENT:
endif

all: diyserver

diyserver: main.c
	$(CC) $(CFLAGS) $(SRCFILES) -o diyserver -lws2_32 -pthread

# clean:
# 	del -f helloworld *.o ||:
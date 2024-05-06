CC=gcc
CFLAGS=-std=c99 -g
SRCFILES= main.c \
json.c server.c http.c files.c \
utils/hashmap.c utils/string_util.c utils/http_parse.c \
types/key_value_pair.c types/http_types.c types/json_types.c 

ifndef VERBOSE
.SILENT:
endif

all: diyserver

diyserver: main.c
	$(CC) $(CFLAGS) $(SRCFILES) -o diyserver -lws2_32 -pthread -ggdb3 -Wall

# clean:
# 	del -f helloworld *.o ||:
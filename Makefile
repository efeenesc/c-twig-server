CC=gcc
CFLAGS=-std=c99 -g
SRCFILES= main.c \
json/json.c json/types/json_types.c \
server/server.c \
http/http.c http/types/http_types.c http/utils/http_parse.c \
files.c \
utils/hashmap.c utils/string_util.c  \
types/key_value_pair.c  \
tls/tls.c tls/types/tls_types.c


ifndef VERBOSE
.SILENT:
endif

all: diyserver

diyserver: main.c
	$(CC) $(CFLAGS) $(SRCFILES) -o twigserver -lws2_32 -pthread -ggdb3

# clean:
# 	del -f helloworld *.o ||:
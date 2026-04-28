CC = gcc
CFLAGS = -Wall -Wextra -o
LIBS = -lssl -lcrypto
.PHONY: all clean

all: mtlsapp

mtlsapp: main.c server.c client.c
	$(CC) $(CFLAGS) mtlsapp main.c server.c client.c $(LIBS)
clean:
	rm -f mtlsapp

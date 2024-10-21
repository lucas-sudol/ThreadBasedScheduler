CC = gcc

CFLAGS = -Wall -g -std=c11 -lpthread -O2
SERIALFLAGS = -Wall -g -std=c11 -O2

A1: main.c queue.c queue.h
	$(CC) $(CFLAGS) main.c queue.c -o A1

serial: serial.c
	$(CC) $(SERIALFLAGS) serial.c -o serial

clean:  
	rm -f A1 serial


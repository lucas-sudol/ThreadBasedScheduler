CC = gcc

CFLAGS = -Wall -g -std=c11 -lpthread

A1: main.c queue.c
	$(CC) $(CFLAGS) main.c queue.c -g -o A1

serial: serial.c
	$(CC) $(CFLAGS) serial.c -o serial

clean:  
	rm -f A1 serial


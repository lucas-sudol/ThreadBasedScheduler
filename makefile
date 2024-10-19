CC = gcc

CFLAGS = -Wall -g -std=c11 -lpthread

A1: main.c
	$(CC) $(CFLAGS) main.c -o A1

serial: serial.c
	$(CC) $(CFLAGS) serial.c -o serial

clean:  
	rm -f A1 serial


CC = gcc

CFLAGS = -Wall -g -std=c11 -lpthread -O0
SERIALFLAGS = -Wall -g -std=c11 -O0

all: pthreads serial

pthreads: main.c queue.c queue.h
	$(CC) $(CFLAGS) main.c queue.c -o pthreads

serial: serial.c
	$(CC) $(SERIALFLAGS) serial.c -o serial

clean:  
	rm -f pthreads serial


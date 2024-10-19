#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h> 

#define MAX_STR 50
#define MAX_THREADS 50

int main(int argc, char **argv) {
    double factor = 1.0;
    double sum = 0.0;
    double pi =0;
    long int n = atoi(argv[1]);

    for(int i = 0; i <n; i++, factor = -factor ) {
        sum += factor/(2*i+1);
    }
    pi = 4.0*sum;
    printf("Pi is %f\n", pi);
}
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    double factor = 1.0;
    double sum = 0.0;
    double pi =0;

    //Check commang line for parced number of iterations
    if(argc < 2) {
        fprintf(stderr, "Error, please include n iterations as command line argument! ./serial n(calculation of pi)\n");
        return 0;
    }

    long int n = strtol(argv[1], NULL, 10); 

    for(int i = 0; i <n; i++, factor = -factor ) {
        sum += factor/(2*i+1);
    }
    pi = 4.0*sum;
    printf("Pi is %.20f\n", pi);

    return 0;
}
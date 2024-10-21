#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <ctype.h>
#include <stdbool.h>
#include "queue.h"

// Global Variables
sem_t availableThreads;
bool printOutput;
bool terminateThreads;

typedef struct {
    int workerId;
    Queue* q;
} ThreadArgs;

// Error executing program
void usage() {
    printf("Error!! To run program: ./A1 fileName flag(true/false) \n");
}

// Calculate Pie Function
double calculatePie(long n) {
    double factor = 1.0;
    double sum = 0.0;

    for (int i = 0; i < n; i++, factor = -factor) {
        sum += factor / (2 * i + 1);
    }
    return ((double)4.0 * sum);
}

// Worker Thread Function
void* workerFunc(void* arg) {
    long value;
    while(getMessage(((ThreadArgs*)arg)->q, &value)) {
        //fprintf(stderr, "Thread %d received value %d\n", ((ThreadArgs*)arg)->workerId, value);
        sem_wait(&availableThreads);
        if(printOutput)
            fprintf(stderr, "Thread %d completed computed Pi using %ld iterations, the result is %.20f\n",((ThreadArgs*)arg)->workerId, value, calculatePie(value));
        else
            calculatePie(value);

        sem_post(&availableThreads);
    }

    //fprintf(stderr, "Thread %d exiting\n", ((ThreadArgs*)arg)->workerId);
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    //local main variables
    FILE* fp = NULL;
    int maxThreads = 0;
    long int tasks = 0;

    //Initialize global variables
    printOutput = false;
    terminateThreads = false;

    //Process arguments
    if (argc > 1 && argc < 4) { // File is present
        fp = fopen(argv[1], "r");
        if (!fp) {
            printf("Could not open file!! \n");
            usage();
            return 1;
        }
    } else { // Incorrect number of arguments
        usage();
        return 1;
    }

    if (argc == 3) { // File and flag
        // Convert flag to lowercase
        for (int i = 0; i < strlen(argv[2]); i++) 
            argv[2][i] = tolower(argv[2][i]);  
        
        if (!strcmp(argv[2], "true")) // If flag is true, print results
            printOutput = true;
    }

    // Parse the file for number of worker threads and messages
    if (!(fscanf(fp, "%d", &maxThreads)== 1) || maxThreads < 1){
        fprintf(stderr, "Error!, Invalid worker threads specified in file!\n");
        return 1;
    }

    //Spawn threads
    pthread_t* tid = malloc(maxThreads * sizeof(pthread_t));
    ThreadArgs* args = malloc(maxThreads * sizeof(ThreadArgs));
    Queue* q = createQueue();
    sem_init(&availableThreads, 0, maxThreads);  

    for (int i = 0; i < maxThreads; i++) {
        args[i].workerId = i + 1;
        args[i].q = q;
        pthread_create(&tid[i], NULL, workerFunc, &args[i]); 
    }

    //Parse tasks and add to queue - after worker pool is created
    while (fscanf(fp, "%ld", &tasks) == 1) {
        sendMessage(q, tasks);
    }

    //Lock mutex before going in to signal any sleeping threads
    pthread_mutex_lock(&q->mutex);
    //Signal all worker threads when finished to exit
    q->exitThread = 1;
    for(int i = 0; i < maxThreads; i++) //Signal any already sleeping threads on empty queue to wakeup
        pthread_cond_signal(&q->cond);

    pthread_mutex_unlock(&q->mutex);

    //Join all threads to ensure completion - avoids busy wait
    for(int i =0; i< maxThreads; i++)
        pthread_join(tid[i], NULL);

    //Clean up
    sem_destroy(&availableThreads);
    fclose(fp);
    free(tid);
    free(args);
    free(q);
    return 0;
}

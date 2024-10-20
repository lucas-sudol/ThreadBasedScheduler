#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>
#include "queue.h"

// Global Variables
sem_t availableThreads;
bool printOutput = false;
bool terminateThreads = false;

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
    return (double)4.0 * sum;
}

// Worker Thread Function
void* workerFunc(void* arg) {
    long value;
    while(getMessage(((ThreadArgs*)arg)->q, &value)) {
        sem_wait(&availableThreads);
        if(printOutput)
            fprintf(stdout, "Thread %d completed computed Pi using %ld iterations, the result is %f\n",((ThreadArgs*)arg)->workerId, value, calculatePie(value));
        else
            calculatePie(value);

        sem_post(&availableThreads);
        pthread_cond_signal(&((ThreadArgs*)arg)->q->endCond);
    }

    fprintf(stderr, "Thread %d exiting\n", ((ThreadArgs*)arg)->workerId);
    return NULL;
}

int main(int argc, char** argv) {
    FILE* fp = NULL;
    int maxThreads = 0;
    long int tasks = 0;

    //Process arguments
    if (argc > 1) { // File is present
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
        for (int i = 0; i < strlen(argv[2]); i++) {
            argv[2][i] = tolower(argv[2][i]);
        }
        
        if (!strcmp(argv[2], "true")) { // If flag is true, print results
            printOutput = true;
        }
    }

    // Parse the file for number of worker threads and messages
    if (!(fscanf(fp, "%d", &maxThreads)== 1) || maxThreads < 1){
        fprintf(stderr, "Error!, Invalid worker threads specified in file!\n");
        return 0;
    }
    


    //Spawn threads
    pthread_t* tid = malloc(maxThreads * sizeof(pthread_t));
    ThreadArgs* args = malloc(maxThreads * sizeof(ThreadArgs));
    Queue* q = createQueue();
    sem_init(&availableThreads, 0, maxThreads);
    
    //Parse tasks and add to queue - before spawning threads to match assignment example
    while (fscanf(fp, "%ld", &tasks) == 1) {
        sendMessage(q, tasks);
    }
    

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    for (int i = 0; i < maxThreads; i++) {
        args[i].workerId = i + 1;
        args[i].q = q;
        pthread_create(&tid[i], &attr, workerFunc, &args[i]); 
    }
    pthread_attr_destroy(&attr);

    //Non busy wait until all threads are finished working on tasks
    int semaphoreCount = 0;
    while(semaphoreCount != maxThreads) {
        sem_getvalue(&availableThreads, &semaphoreCount);
        //Conditional wait until threads finish, then check if all tasks are complete
        if(semaphoreCount != maxThreads){
            pthread_mutex_lock(&q->mutex);
            pthread_cond_wait(&q->endCond, &q->mutex);
            pthread_mutex_unlock(&q->mutex);
        }
    }



    q->exitThread = 1;
    for(int i = 0; i < maxThreads; i++) 
        pthread_cond_signal(&q->cond);

    //Clean up

    sem_destroy(&availableThreads);
    fclose(fp);
    free(tid);
    free(args);
    free(q);

    

    /*
    // Initialize counting semaphore for available worker threads
    sem_init(&availableThreads, 0, maxThreads);

    // Wait until semaphore is == maxThreads, then signal termination
    terminateThreads = true;
    */
    return 0;
}

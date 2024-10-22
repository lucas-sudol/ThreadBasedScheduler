#include <stdlib.h>
#include <pthread.h>

typedef struct node {
    long value;
    struct node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    pthread_mutex_t mutex;
    pthread_mutex_t printMutex;
    pthread_cond_t cond;
    pthread_cond_t endCond;
    int exitThread;
} Queue;

Queue* createQueue();
void freeQueue(Queue* q);
void sendMessage(Queue* q, long value);
int getMessage(Queue* q, long* outputValue);
// Adapted from msgqueueCondition.c by Professor Denis Nikitenko
#include "queue.h"

//Create a queue and initilize its mutex and condition variable
Queue* createQueue()
{
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->head = NULL;
    q->tail = NULL;
    pthread_mutex_init(&q->mutex, NULL);
    
    //Initialize the condition variable
    pthread_cond_init(&q->cond, NULL);
    pthread_cond_init(&q->endCond, NULL);
    q->exitThread = 0;
    return q;
}

//"Send" a message - append it onto the queue
void sendMessage(Queue* q, long value)
{
    Node* node = (Node*)malloc(sizeof(Node));
    node->value = value;
    node->next = NULL;

    // critical section
    pthread_mutex_lock(&q->mutex);
    if (q->tail != NULL) {
        q->tail->next = node;       // append after tail
        q->tail = node;
    } else {
        q->tail = node;
        q->head = node;   // first node
    }
    //Signal the consumer thread waiting on this condition variable
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

//"Receive" a message - remove it from the queue
int getMessage(Queue* q, long* outputValue)
{
    int success = 0;
    
    // critical section
    pthread_mutex_lock(&q->mutex);
    
    //Wait for a signal telling us that there's something on the queue
    //If we get woken up but the queue is still null, we go back to sleep
    while(q->head == NULL){
        //Check terminate threads condition, return 0 if we need to terminate
        if(q->exitThread) {
            pthread_mutex_unlock(&q->mutex);
            return 0;
        }
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    
    //By the time we get here, we know q->head is not null, so it's all good
    Node* oldHead = q->head;
    *outputValue = oldHead->value;    // copy out the message
    q->head = oldHead->next;
    if (q->head == NULL) {
        q->tail = NULL;         // last node removed
    }
    free(oldHead);
    success = 1;
    
    //Release lock
    pthread_mutex_unlock(&q->mutex);
    return success;
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct mcs_node {
    struct mcs_node* next;
    int locked;
} mcs_node_t;

typedef struct mcs_lock {
    mcs_node_t* tail;
} mcs_lock_t;

mcs_lock_t lock; // Global lock variable
pthread_key_t my_node_key; // Thread-specific key
volatile int counter = 0; // Shared variable
const int NUM_OPERATIONS = 1000000; // Number of operations

void mcs_lock(mcs_lock_t* lock, mcs_node_t* my_node) {
    my_node->next = NULL;
    my_node->locked = 1;
    mcs_node_t* prev_node = __sync_lock_test_and_set(&lock->tail, my_node);
    if (prev_node != NULL) {
        prev_node->next = my_node;
        while (my_node->locked) {}
    }
}

void mcs_unlock(mcs_node_t* my_node) {
    if (my_node->next == NULL) {
        if (__sync_bool_compare_and_swap(&lock.tail, my_node, NULL)) {
            return;
        }
        while (my_node->next == NULL) {}
    }
    my_node->next->locked = 0;
}

void cleanup(void* arg) {
    free(arg);
}

void* test_thread(void* arg) {
    int* thread_id = (int*)arg;
    mcs_node_t* my_node = malloc(sizeof(mcs_node_t));
    pthread_setspecific(my_node_key, my_node);

    for (int i = 0; i < NUM_OPERATIONS; i++) {
        // Acquire lock
        mcs_lock(&lock, my_node);

        // Critical section
        counter++;

        // Release lock
        mcs_unlock(my_node);
    }

    return NULL;
}

int main() {
    // Initialize the lock
    lock.tail = NULL;

    // Create thread-specific key
    pthread_key_create(&my_node_key, cleanup);

    // Create threads
    const int NUM_THREADS = 4;
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, test_thread, &thread_ids[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end_time, NULL);
    long start_micros = start_time.tv_sec * 1000000 + start_time.tv_usec;
    long end_micros = end_time.tv_sec * 1000000 + end_time.tv_usec;
    long elapsed_micros = end_micros - start_micros;

    printf("Number of operations: %d\n", NUM_OPERATIONS * NUM_THREADS);
    printf("Shared variable (Counter) value: %d\n", counter);
    printf("Throughput: %.2f ops/sec\n", (double)(NUM_OPERATIONS * NUM_THREADS) / (double)elapsed_micros * 1000000);

    return 0;
}


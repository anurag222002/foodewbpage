#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Declare two semaphores and shared variables
sem_t mutex, writeblock;
int data = 0, rcount = 0;

// Reader thread function
void *reader(void *arg) {
    int f;
    f = (int)arg;

    // Acquire the mutex to ensure exclusive access to rcount
    sem_wait(&mutex);
    rcount = rcount + 1;

    // If this is the first reader, block writers from accessing data
    if (rcount == 1)
        sem_wait(&writeblock);

    // Release the mutex and read the shared data
    sem_post(&mutex);
    printf("Data read by the reader %d is %d\n", f, data);
    sleep(1);

    // Acquire the mutex to update rcount
    sem_wait(&mutex);
    rcount = rcount - 1;

    // If this is the last reader, allow writers to access data
    if (rcount == 0)
        sem_post(&writeblock);

    // Release the mutex
    sem_post(&mutex);
}

// Writer thread function
void *writer(void *arg) {
    int f;
    f = (int)arg;

    // Acquire the writeblock semaphore to block other writers
    sem_wait(&writeblock);

    // Modify the shared data
    data++;

    // Print the updated data and sleep
    printf("Data written by the writer %d is %d\n", f, data);
    sleep(1);

    // Release the writeblock semaphore to allow other writers
    sem_post(&writeblock);
}

int main() {
    int i;
    pthread_t rtid[5], wtid[5];

    // Initialize semaphores and shared variables
    sem_init(&mutex, 0, 1);
    sem_init(&writeblock, 0, 1);

    // Create reader and writer threads
    for (i = 0; i < 3; i++) {
        pthread_create(&wtid[i], NULL, writer, (void *)i);
        pthread_create(&rtid[i], NULL, reader, (void *)i);
    }

    // Wait for all threads to complete
    for (i = 0; i < 3; i++) {
        pthread_join(wtid[i], NULL);
        pthread_join(rtid[i], NULL);
    }

    return 0;
}

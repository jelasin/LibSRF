#ifndef __SHMFIFO_H__
#define __SHMFIFO_H__

#include "ring_queue.h"
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>

#define SEM_FIFO_NAME "/sem_to_sync"    // semaphore for synchronization ; idx = 0
#define SEM_FULL_NAME "/sem_is_full"    // wether the queue is full ==> how much space is available ; idx = 1
#define SEM_EMPTY_NAME "/sem_is_empty"  // wether the queue is empty ==> how much space has been used ; idx = 2

typedef struct {
    sem_t *sem_id[3];           // Semaphore ID
    int blksz;                  // Shared memory block size
    int blknum;                 // Shared memory block number
    int wpos;                   // Write position
    int rpos;                   // Read position
    ring_queue_t *ring_queue;   // Pointer to ring queue structure
} shmfifo_head_t;

typedef struct {
    shmfifo_head_t *p_head;      // Pointer to shared memory header
    char *p_data;                // Pointer to shared memory data
} shmfifo_t;

int shmfifo_init(shmfifo_t *shm, size_t blknum, size_t blksz);

void shmfifo_destroy(shmfifo_t *shm);

int shmfifo_write(shmfifo_t *shm, void *element, size_t size);

int shmfifo_read(shmfifo_t *shm, void *element, size_t size);

#endif /* __SHMFIFO_H__ */
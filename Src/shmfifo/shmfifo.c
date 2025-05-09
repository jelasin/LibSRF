#include "shmfifo.h"

#ifdef DEBUG
#include <stdio.h>
#include <errno.h>
#endif

#include <string.h>

static int g_block_size = 0;

static void shmfifo_destroy_element(void *element)
{
    munmap(element, g_block_size);
}

int shmfifo_init(shmfifo_t *shm, size_t blknum, size_t blksz)
{
    if (shm == NULL || blknum == 0 || blksz == 0) 
    {
#ifdef DEBUG
        perror("[shmfifo_init] Invalid parameters");
#endif
        return -1; // Invalid parameters
    }
    shm->p_head = malloc(sizeof(shmfifo_head_t));
    if (shm->p_head == NULL) 
    {
#ifdef DEBUG
        perror("[shmfifo_init] Memory allocation failed");
#endif
        return -1; // Memory allocation failed
    }
    g_block_size = blksz;
    shm->p_head->blksz = blksz;
    shm->p_head->blknum = blknum;

    shm->p_head->sem_id[0] = sem_open(SEM_FIFO_NAME, O_CREAT, 0644, 1);
    shm->p_head->sem_id[1] = sem_open(SEM_FULL_NAME, O_CREAT, 0644, blknum);
    shm->p_head->sem_id[2] = sem_open(SEM_EMPTY_NAME, O_CREAT, 0644, 0);
    
    if (shm->p_head->sem_id[0] == SEM_FAILED ||
        shm->p_head->sem_id[1] == SEM_FAILED ||
        shm->p_head->sem_id[2] == SEM_FAILED) 
    {
#ifdef DEBUG
        perror("[shmfifo_init] Semaphore creation failed");
#endif
        return -1; // Semaphore creation failed
    }

    shm->p_head->ring_queue = ring_queue_create(blknum, shmfifo_destroy_element);
    if (shm->p_head->ring_queue == NULL) 
    {
#ifdef DEBUG
        perror("[shmfifo_init] Ring queue creation failed");
#endif
        return -1; // Ring queue creation failed
    }    

    char shm_name[64];
    memset(shm_name, '\x00', 64);
    for (size_t i = 0; i < blknum; i++) 
    {
        snprintf(shm_name, 64, "/shm_fifo_%ld", i);
        int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0644);
        if (shm_fd == -1) 
        {
#ifdef DEBUG
            perror("[shmfifo_init] Shared memory creation failed");
            #endif
            return -1; // Shared memory creation failed
        }
        ftruncate(shm_fd, blksz);
        void *shm_ptr = mmap(NULL, blksz, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (shm_ptr == MAP_FAILED) 
        {
#ifdef DEBUG
            perror("[shmfifo_init] Memory mapping failed");
#endif
            return -1; // Memory mapping failed
        }
        ring_queue_enqueue(shm->p_head->ring_queue, shm_ptr);
        close(shm_fd);
        memset(shm_name, 0, 64);
    }
    shm->p_data = shm->p_head->ring_queue->buffer[0];
    shm->p_head->wpos = shm->p_head->rpos = 0;

    return 0; // Initialization successful
}

void shmfifo_destroy(shmfifo_t *shm)
{
    if (shm == NULL) 
    {
#ifdef DEBUG
        perror("[shmfifo_destroy] Invalid parameters");
#endif
        return; // Invalid parameters
    }
    
    for (int i = 0; i < shm->p_head->blknum; i++) 
    {
        char shm_name[64];
        snprintf(shm_name, 64, "/shm_fifo_%d", i);
        if (shm_unlink(shm_name) == -1) 
        {
#ifdef DEBUG
            perror("[shmfifo_destroy] Shared memory unlink failed");
#endif
            // continue; // Shared memory unlink failed
        }
    }

    if (sem_close(shm->p_head->sem_id[0]) == -1 || 
        sem_close(shm->p_head->sem_id[1]) == -1 || 
        sem_close(shm->p_head->sem_id[2]) == -1) 
    {
#ifdef DEBUG
        perror("[shmfifo_destroy] Semaphore close failed");
#endif
    }

    ring_queue_destroy(shm->p_head->ring_queue);

    free(shm->p_head);
    shm->p_head = NULL;
    shm->p_data = NULL;
}

int shmfifo_write(shmfifo_t *shm, void *element, size_t size)
{
    if (shm == NULL || element == NULL || size == 0) 
    {
#ifdef DEBUG
        perror("[shmfifo_write] Invalid parameters");
#endif
        return -1; // Invalid parameters
    }

    if (size > (size_t)shm->p_head->blksz) 
    {
#ifdef DEBUG
        perror("[shmfifo_write] Element size exceeds block size");
#endif
        return -1; // Element size exceeds block size
    }

    // Wait for available space
    sem_wait(shm->p_head->sem_id[1]);

    // Lock the FIFO
    sem_wait(shm->p_head->sem_id[0]);

    // Write the data
    shm->p_data = shm->p_head->ring_queue->buffer[shm->p_head->wpos];
    if (shm->p_data == NULL)
    {
#ifdef DEBUG
        perror("[shmfifo_write] Failed to get shared memory pointer");
#endif
        sem_post(shm->p_head->sem_id[0]); // Unlock the FIFO
        sem_post(shm->p_head->sem_id[1]); // Signal that space is available
        return -1; // Failed to get shared memory pointer
    }
    // Copy the data to the shared memory
    memcpy(shm->p_data, element, size);
    shm->p_head->wpos = (shm->p_head->wpos + 1) % (shm->p_head->blknum);

    // Unlock the FIFO
    sem_post(shm->p_head->sem_id[0]);

    // Signal that data is available
    sem_post(shm->p_head->sem_id[2]);

    return 0; // Write successful
}

int shmfifo_read(shmfifo_t *shm, void *element, size_t size)
{
    if (shm == NULL || element == NULL || size == 0)
    {
#ifdef DEBUG
        perror("[shmfifo_read] Invalid parameters");
#endif
        return -1; // Invalid parameters
    }

    // Wait for data to be available
    sem_wait(shm->p_head->sem_id[2]);

    // Lock the FIFO
    sem_wait(shm->p_head->sem_id[0]);

    // Read the data
    shm->p_data = shm->p_head->ring_queue->buffer[shm->p_head->rpos];
    if (shm->p_data == NULL) 
    {
#ifdef DEBUG
        perror("[shmfifo_read] Failed to get shared memory pointer");
#endif
        sem_post(shm->p_head->sem_id[0]); // Unlock the FIFO
        sem_post(shm->p_head->sem_id[2]); // Signal that data is available
        return -1; // Failed to get shared memory pointer
    }
    // Copy the data from the shared memory
    memcpy(element, shm->p_data, size);
    shm->p_head->rpos = (shm->p_head->rpos + 1) % (shm->p_head->blknum);
    memset(shm->p_data, 0, shm->p_head->blksz); // Clear the data in shared memory
    // Unlock the FIFO
    sem_post(shm->p_head->sem_id[0]);

    // Signal that space is available
    sem_post(shm->p_head->sem_id[1]);

    return 0; // Read successful
}

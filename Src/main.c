#include "shmfifo.h"
#include <stdio.h>

int main()
{
    shmfifo_t shm;
    size_t blknum = 10; // Number of blocks
    size_t blksz = 1024; // Size of each block

    // Initialize the shared memory FIFO
    if (shmfifo_init(&shm, blknum, blksz) != 0) {
        return -1; // Initialization failed
    }

    // Perform read/write operations...
    shmfifo_write(&shm, "Hello, World!", 13);
    shmfifo_write(&shm, "Hello, World?", 13);
    char buffer[1024];
    shmfifo_read(&shm, buffer, sizeof(buffer));
    printf("Read from FIFO: %s\n", buffer);
    shmfifo_read(&shm, buffer, sizeof(buffer));
    printf("Read from FIFO: %s\n", buffer);
    shmfifo_read(&shm, buffer, sizeof(buffer));
    printf("Read from FIFO: %s\n", buffer);
    // Destroy the shared memory FIFO
    shmfifo_destroy(&shm);

    return 0; // Success
}
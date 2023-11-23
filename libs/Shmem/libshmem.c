#include "libshmem.h"
#include <errno.h>

int get_shared_block(char *filename, int size) {
    key_t key;

    key = ftok(filename, 0);
    if(key < 0){
        return -1;
    }

    return shmget(key, size, 0644 | IPC_CREAT);
}

void *attach_memory_block(char *filename, int size) {
    int shared_block_id = get_shared_block(filename, size);
    void* result;

    if(shared_block_id < 0){
        return NULL;
    }

    result = shmat(shared_block_id, NULL, 0);
    if(result < 0){
        return NULL;
    }

    return result;
}

int detach_memory_block(void *block){
    return (shmdt(block) != -1);
}

int destroy_memory_block(char *filename){
    int shared_block_id = get_shared_block(filename, 0);

    if(shared_block_id < 0){
        return -1;
    }
    return (shmctl(shared_block_id, IPC_RMID, NULL) != -1);
}
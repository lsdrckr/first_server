#ifndef __SHMEM__
#define __SHMEM__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

int get_shared_block(char *filename, int size);
void *attach_memory_block(char *filename, int size);
int detach_memory_block(void *block);
int destroy_memory_block(char *filename);
#endif
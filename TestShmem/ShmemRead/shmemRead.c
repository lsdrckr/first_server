#include "libshmem.h"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define FILENAME "../ShmemWrite/shmemWrite.c"

void *block;

void sig_handler() {
    printf("Stoping process ...\n");
    detach_memory_block(block);
    if (destroy_memory_block(FILENAME))
    {
        printf("Destroyed block : %s\n", FILENAME);
    }else{
        printf("Could not destroy block : %s\n", FILENAME);
    }
    exit(EXIT_SUCCESS);
}

int main(){

    block = attach_memory_block(FILENAME, 4096);

    if (block == NULL){
        printf("ERROR: couldn't get block\n");
        return -1;
    }

    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        perror("Erreur lors de la configuration du gestionnaire pour SIGINT");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTSTP, sig_handler) == SIG_ERR) {
        perror("Erreur lors de la configuration du gestionnaire pour SIGSTOP");
        exit(EXIT_FAILURE);
    }

    while(1){
        printf("Reading : %d\n", *((int *)block));
        sleep(1);
    }
}
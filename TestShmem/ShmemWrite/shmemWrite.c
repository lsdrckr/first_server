#include "libshmem.h"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void *block;

void sig_handler() {
    printf("\nStoping process ...\n");
    detach_memory_block(block);
    exit(EXIT_SUCCESS);
}

int main(){

    block = attach_memory_block("shmemWrite.c", 4096);
    *((int*)block) = 0;

    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        perror("Erreur lors de la configuration du gestionnaire pour SIGINT");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTSTP, sig_handler) == SIG_ERR) {
        perror("Erreur lors de la configuration du gestionnaire pour SIGSTOP");
        exit(EXIT_FAILURE);
    }

    if (block == NULL){
        printf("ERROR: couldn't get block\n");
        return -1;
    }

    while(1){
        *((int*)block) = *((int*)block) + 1;
        printf("Writing : %d\n", *((int*)block));
        sleep(1);
    }
}
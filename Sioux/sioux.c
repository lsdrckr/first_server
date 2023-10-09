#include <stdio.h>
#include "libreseau.h"
#include <stdlib.h>

int clientGestion(){
    printf("Un nouveau client !\n");
    return 0;
}

int main(){

    int sockFd;

    sockFd = serverInit("8080", 3);
    if(sockFd < 0){
        perror("Port non utilisable\n");
        exit(EXIT_FAILURE);
    }
    printf("Serveur initialisé\n");

    printf("Serveur à l'écoute ...\n");
    serverLoop(sockFd, clientGestion);


    return 0;
}

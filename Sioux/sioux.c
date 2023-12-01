#include <stdio.h>
#include "libreseau.h"
#include "analyste_http.h"
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include "libshmem.h"


#define SHMEM_NAME "../Ablette/ablette.c"
#define MAX_SERVICE_NAME 32

void sig_handler() {
    printf("Stoping process ...\n");
    if (destroy_memory_block(SHMEM_NAME))
    {
        printf("Destroyed block : %s\n", SHMEM_NAME);
    }else{
        printf("Could not destroy block : %s\n", SHMEM_NAME);
    }
    exit(EXIT_SUCCESS);
}

void analyzeArg(int argc, char* argv[], char service[]){
    
    int option;
    
    static struct option long_options[] = {
        {"port", required_argument, NULL, 'p'}
    };
    while((option = getopt_long(argc, argv, "p:", long_options, NULL)) != -1){
        switch(option) {
            case 'p':
                strcpy(service,optarg);
                break;
            default : 
                perror("Arguments incorrects -p [port] --port [port]");
                exit(EXIT_FAILURE);
        }
    }
}

int clientHandler(int sockFd){

    printf("Nouvelle connexion:\n");
    // Obtenir la strcuture de fichier
    FILE *stream = fdopen(sockFd, "a+");
    if(stream==NULL){
        perror("clientGestion.fdopen");
        exit(EXIT_FAILURE);
    }

    requestHandler(stream);

    // Terminer la connexion
    fclose(stream);
    return 0;
}

int main(int argc, char* argv[]){

    int sockFd;
    char service[MAX_SERVICE_NAME] = "80";
    
    // Initialisation des interruptions 
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        perror("Erreur lors de la configuration du gestionnaire pour SIGINT");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTSTP, sig_handler) == SIG_ERR) {
        perror("Erreur lors de la configuration du gestionnaire pour SIGSTOP");
        exit(EXIT_FAILURE);
    }

    analyzeArg(argc, argv, service);
    
    sockFd = serverInit(service, 3);
    if(sockFd < 0){
        perror("Port non utilisable\n");
        exit(EXIT_FAILURE);
    }
    printf("Serveur initialisé sur le port %s\n", service);

    printf("Serveur à l'écoute ...\n");
    printf("\n\n");
    serverLoop(sockFd, clientHandler);

    return 0;
}

#include <stdio.h>
#include "libreseau.h"
#include "analyste_http.h"
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define MAX_SERVICE_NAME 32

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

    printf("Nouvelle connexion :\n");
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

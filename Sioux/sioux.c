#include <stdio.h>
#include "libreseau.h"
#include "http.h"
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define MAX_SERVICE_NAME 32
#define MAX_LINE 512

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
    serverLoop(sockFd, clientGestion);


    return 0;
}

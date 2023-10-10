#include <stdio.h>
#include "libreseau.h"
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define MAX_SERVICE_NAME 32
#define DEFAULT_PORT 8080

#define MAX_LIGNE 16
#define BUFFERSIZE 512

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

int clientGestion(int sockFd){
    
    printf("Un nouveau client !\n");
    // Obtenir la strcuture de fichier
    FILE *stream = fdopen(sockFd, "a+");
    if(stream==NULL){
        perror("clientGestion.fdopen");
        exit(EXIT_FAILURE);
    }
    
    // Obtenir le ficher html
    FILE *html = fopen("../Web/index.html", "r");
    if(html == NULL){
        perror("Ouverture du fichier html");
        exit(EXIT_FAILURE);
    }
    
    //Envoyer l'entête
    fprintf(stream, "200\n");
    fprintf(stream, "Content-Type: text/html\n");
    fprintf(stream, "Content-Length: 90\n");
    
    char ligne[MAX_LIGNE];
    while(fgets(ligne,MAX_LIGNE,html)!=NULL)
    fprintf(stream,"%s",ligne);
    
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
    serverLoop(sockFd, clientGestion);


    return 0;
}

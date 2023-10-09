#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <string.h>
#include "libserver.h"

void error(const char * msg){
    perror(msg);
    exit(EXIT_FAILURE);
}


int serverInit(char *service, int connections){

    struct addrinfo precisions, *result, *origine;
    int sockFd;
    int status;

    // Construction de la structure addr

    memset(&precisions,0,sizeof precisions);
    precisions.ai_family=AF_UNSPEC;
    precisions.ai_socktype=SOCK_STREAM;
    precisions.ai_flags=AI_PASSIVE;

    status=getaddrinfo(NULL,service,&precisions,&origine);
    if(status<0) error("serverInit.getaddrinfo");

    struct addrinfo *p;
    for(p=origine,result=origine;p!=NULL;p=p->ai_next){
        if(p->ai_family==AF_INET6){
            result=p;
            break;
        }
    }

    // Création d'une socket
    sockFd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(sockFd < 0) error("serverInit.socket");

    /* Options utiles */
    int vrai=1;
    if(setsockopt(sockFd,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
        error("serverInit.setsockopt (REUSEADDR)");
    }
    if(setsockopt(sockFd,IPPROTO_TCP,TCP_NODELAY,&vrai,sizeof(vrai))<0){
        error("serverInit.setsockopt (NODELAY)");
    }

    /* Specification de l'adresse de la socket */
    status=bind(sockFd,result->ai_addr,result->ai_addrlen);
    if(status<0){ freeaddrinfo(origine); shutdown(sockFd,SHUT_RDWR); return -1; }

    /* Liberation de la structure d'informations */
    freeaddrinfo(origine);

    /* Taille de la queue d'attente */
    status=listen(sockFd,connections);
    if(status<0){ shutdown(sockFd,SHUT_RDWR); return -2; }

    return sockFd;
}

int serverLoop(int sockFd, int (*traitement)(int)){
    int streamFd;
    while(1){
        // Attente d'une connexion
        streamFd = accept(sockFd, NULL, NULL);
        if(streamFd < 0) return -1;

        // Passage de la socket de dialogue à la fonction de traitement
        if(traitement(streamFd)<0){
            shutdown(sockFd,SHUT_RDWR);
            return 0;
        }
    }
}

#include "analyste_http.h"

int clientGestion(int sockFd){

    printf("Un nouveau client !\n");
    // Obtenir la strcuture de fichier
    FILE *stream = fdopen(sockFd, "a+");
    if(stream==NULL){
        perror("clientGestion.fdopen");
        exit(EXIT_FAILURE);
    }

    // Obtenir le ficher html
    FILE *html = fopen("../html/index.html", "r");
    if(html == NULL){
        perror("Ouverture du fichier html");
        exit(EXIT_FAILURE);
    }

    //Envoyer l'entête
    fprintf(stream, "HTTP/1.1 200 OK\r\n");
    fprintf(stream, "Content-Type: text/html\r\n");
    fprintf(stream, "Content-Length: 512\r\n");
    fprintf(stream, "\r\n");

    char line[MAX_LINE];
    while(fgets(line, MAX_LINE, html)!=NULL){
        fprintf(stream, "%s",line);
    }

    // Terminer la connexion
    printf("Envoie de la page web à l'utilisateur fin du stream\n");
    fclose(stream);
    return 0;
}

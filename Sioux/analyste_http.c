#include "analyste_http.h"
#define MAX_LINE 512

void sendHtml(FILE *stream, char* htmlPath){
    // Obtenir le ficher html
    FILE *htmlFile = fopen(htmlPath, "r");
    if(htmlFile == NULL){
        perror("Ouverture du fichier html");
        exit(EXIT_FAILURE);
    }

    //Obtenir la taille du fichier
    long savePos = ftell(htmlFile);
    fseek(htmlFile, 0, SEEK_END);
    long size = ftell(htmlFile);
    fseek(htmlFile, savePos, SEEK_SET);

    //Envoyer l'entête
    fprintf(stream, "HTTP/1.1 200 OK\r\n");
    fprintf(stream, "Content-Type: text/html\r\n");
    fprintf(stream, "Content-Length: %ld\r\n", size);
    fprintf(stream, "\r\n");

    //Envoyer le corps
    char c;
    while((c = fgetc(htmlFile)) != EOF){
        fputc(c, stream);
    }

    printf("Html send success !\n");

    //Fermeture de l'html
    fclose(htmlFile);
}

void requestHandler(FILE *stream){
    sendHtml(stream, "../html/vote.html");
}

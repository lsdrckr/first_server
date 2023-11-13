#include "analyste_http.h"

int sendHtml(FILE *stream, char* htmlPath){
    // Obtenir le ficher html
    FILE *htmlFile = fopen(htmlPath, "r");
    if(htmlFile == NULL){
        return -1;
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

    //Fermeture de l'html
    fclose(htmlFile);
    return 0;
}

// Remplace l'ensemble des char toremove par les char toPlace renvoie 1 si au moins un remplacement 0 sinon
int subCharInString(char* line, char toRemove, char toPlace){
    int flag = 0;
    int i=0;
    while (line[i] != '\0'){
        if (line[i] == toRemove){
            line[i] = toPlace;
            flag = 1;
        }
        i++;
    }
    return flag;
}

void getMethod(char* request, char* method){
    sscanf(request,"%s", method);
}

int getPath(char* request, char* csvPath, char* extension){
    char surplus[MAX_LINE];
    char tmp[MAX_LINE];

    sscanf(request, "%s %s",surplus, tmp);
    if(subCharInString(tmp, '.', ' ') == 0) return 0;
    sscanf(tmp,"%s",tmp);

    strcpy(csvPath, "../");
    strcat(csvPath, extension);
    strcat(csvPath, tmp);
    strcat(csvPath, ".");
    strcat(csvPath, extension);
    return 1;
}

int getArg(char* request, char* arg){
    char surplus[MAX_LINE];
    char tmp[MAX_LINE];

    sscanf(request, "%s %s", surplus, tmp);

    if(subCharInString(tmp, '?', ' ') == 0){
        return 0;
    }

    sscanf(tmp, "%s %s", surplus, arg);
    subCharInString(arg,'=', ';');
    subCharInString(arg, '&', ';');
    return 1;
}

int appendToCsv(char* csvPath, char* arg){

    int csvFile;

    // Ouverture ou création du fichier csv
    mkdir("../csv", S_IRWXU | S_IRGRP | S_IROTH | S_IXGRP | S_IXOTH);
    csvFile = open(csvPath, O_CREAT|O_WRONLY|O_APPEND, S_IROTH|S_IWOTH);
    chmod(csvPath, 0666);
    if(csvFile < 0){
        return -1;
    }

    // Ecriture des arguments dans le csv
    strcat(arg,"\n");
    write(csvFile, arg, strlen(arg));

    // Fermeture du fichier csv
    close(csvFile);
    return 0;
}

void requestHandler(FILE *stream){
    
    char request[MAX_LINE];
    char method[MAX_LINE];
    char htmlPath[MAX_LINE];
    char csvPath[MAX_LINE];
    char arg[MAX_LINE];

    // Récupération de la requète et analyse de la requète
    fgets(request,MAX_LINE,stream);
    getMethod(request, method);

    if(strcmp(method,"GET") == 0){

        printf("\tGestion de la requète GET : %s", request);

        if(getArg(request, arg)){

            printf("\tPage Reponse\n");

            // Récupération du chemin csv
            getPath(request, csvPath, "csv");

            // ajout dans le csv
            if(appendToCsv(csvPath, arg) < 0){

                printf("\tImpossible d'ouvrir le fichier csv\n");

            }else{

                printf("\tReponse enregistrer dans %s\n", csvPath);

            }
        }else{

            printf("\tPage vote\n");

        }

        // Récupération du chemin html
        if(getPath(request, htmlPath, "html") == 0){
            strcpy(htmlPath, "../html/vote.html");
        }

        // Envoie de l'html
        if(sendHtml(stream, htmlPath) < 0){

            printf("\tImpossible d'ouvrir le fichier %s\n", htmlPath);

        }else{

            printf("\t%s bien envoyé\n", htmlPath);

        }
    }else{

        printf("Méthode non reconnue pour la requète %s\n", request);

    }
}

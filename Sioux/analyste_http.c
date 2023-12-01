#include "analyste_http.h"
#include "libshmem.h"
#include <netinet/ip.h>

#define SHMEM_NAME "../Ablette/ablette.c"
#define NB_TOP_ADDR 5
#define ADDR_STRING_SIZE 16

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

void sendStats(FILE *stream){
    // Récupération de la mémoire partagée
    void *sh_address = attach_memory_block(SHMEM_NAME, 4096);

    if (sh_address == NULL){
        printf("ERROR: couldn't get memory block\n");
        exit(EXIT_FAILURE);
    }

    char topAddressString[NB_TOP_ADDR][ADDR_STRING_SIZE];

    memcpy(topAddressString, sh_address, sizeof(topAddressString));

    //Envoyer l'entête
    fprintf(stream, "HTTP/1.1 200 OK\r\n");
    fprintf(stream, "Content-Type: text/html\r\n");
    fprintf(stream, "Content-Length: %d\r\n", MAX_STATS_SIZE);
    fprintf(stream, "\r\n");

    //Envoyer le corps
    fprintf(stream, "<body>");
    fprintf(stream, "<p>Top %d des adresses IP sollicitant le serveur le plus frequemment", NB_TOP_ADDR);
    for(int i=0; i<NB_TOP_ADDR; i++){ 
        fprintf(stream,"<p>%d: %s", i+1, topAddressString[i]);
    }

    // détacher les block mémoires
    detach_memory_block(sh_address);
}

// Remplace l'ensemble des char toremove par les char toPlace renvoie 1 si au moins un remplacement 0 sinon
int switchCharInString(char* line, char toRemove, char toPlace){
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
    if(switchCharInString(tmp, '.', ' ') == 0) return 0;
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

    if(switchCharInString(tmp, '?', ' ') == 0){
        return 0;
    }

    sscanf(tmp, "%s %s", surplus, arg);
    switchCharInString(arg,'=', ';');
    switchCharInString(arg, '&', ';');
    return 1;
}

void getFileExtension(FILE *stream, char* request, char* fileExtension){
    char surplus[MAX_LINE];
    char tmp[MAX_LINE];

    sscanf(request, "%s %s", surplus, tmp);

    switchCharInString(tmp, '?', ' ');
    if(switchCharInString(tmp, '.', ' ') == 0){
        if(strcmp(tmp, "/") == 0){
            strcpy(fileExtension, "html");
        }else if(strcmp(tmp, "/stats") == 0){
            sendStats(stream);
            fileExtension = NULL;
        }else{
            fileExtension = NULL;
        }
        
    }else{
        sscanf(tmp, "%s %s", surplus, fileExtension);
    }
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
    char path[MAX_LINE];
    char csvPath[MAX_LINE];
    char arg[MAX_LINE];
    char fileExtension[MAX_LINE];

    // Récupération de la requète et analyse de la requète
    fgets(request,MAX_LINE,stream);
    getMethod(request, method);

    if(strcmp(method,"GET") == 0){

        printf("\tGestion de la requète GET : %s", request);
        getFileExtension(stream, request, fileExtension);

        if(strcmp(fileExtension,"html") == 0){

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

            // Récupération du chemin
            if(getPath(request, path, fileExtension) == 0){
                strcpy(path, "../html/vote.html");             //chemin par Defaut
            }

            // Envoie du fichier
            if(strcmp(path, "../html/stats.html") == 0){
                sendStats(stream);
            }else{
                if(sendHtml(stream, path) < 0){
                    printf("\tImpossible d'ouvrir le fichier %s\n", path);
                }else{
                    printf("\t%s bien envoyé\n", path);
                }
            }

        }else{
            printf("\tAucun html à envoyer\n");
        }
    }else{

        printf("Méthode non reconnue pour la requète %s\n", request);

    }
}

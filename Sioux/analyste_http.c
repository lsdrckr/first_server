#include "analyste_http.h"

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

int isForm(char* line){
    int i=0;
    while (line[i] != '\0'){
        if (line[i] == '?'){
            line[i] = ' ';
            return 1;
        }
        i++;
    }
    return 0;
}

void requestHandler(FILE *stream){
    
    char line[MAX_LINE];
    fgets(line,MAX_LINE,stream);
    
    char method[MAX_LINE];
    char urlAndArg[MAX_LINE];
    char version[MAX_LINE];
    char url[MAX_LINE];
    char arg[MAX_LINE];
    char csvPath[MAX_LINE];
    sscanf(line, "%s %s %s", method, urlAndArg, version);
    
    if(strcmp(method,"GET") == 0){
        printf("Gestion de la requète GET :\n");
        
        if(isForm(urlAndArg)){
            sscanf(urlAndArg, "%s %s", url, arg);
            strcpy(csvPath, url);
            char csvPath[MAX_LINE];
            int i=0;
                while (csvPath[i] != '\0'){
                    if (csvPath[i] == '.'){
                    csvPath[i] = ' ';
                    }
                    i++;
                }
            sscanf(csvPath, "%s", csvPath);
            strcat(csvPath, ".csv");
            printf("%s\n", csvPath);
        }else{
            sscanf(urlAndArg, "%s", url);
        }
        char htmlPath[MAX_LINE] = "../html";
        strcat(htmlPath, url);
        sendHtml(stream, htmlPath);
    }
    
    
//     if(strcmp(token,"GET") == 0){
//         token = strtok(NULL, " ");
//         token = strtok(token, "?");
//         char htmlFile[MAX_LINE] = "../html";
//         strcat(htmlFile, token);
//         
//         sendHtml(stream, htmlFile);
//     }else{
//         
//     }
}

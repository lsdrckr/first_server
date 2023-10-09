int clientGestion(){
    printf("Un nouveau client !\n");
    return 0;
}

int main(){

    int sockFd;

    sockFd = serverInit("8080", 3);
    if(sockFd < 0){
        error("Port non utilisable\n");
    }
    printf("Serveur initialisé\n");

    printf("Serveur à l'écoute ...\n");
    serverLoop(sockFd, clientGestion);


    return 0;
}

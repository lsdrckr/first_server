#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <pcap.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_SERVICE_NAME 32
#define MAX_PORTS 32
#define MAX_FILTER_EXP 1024
#define MAX_ADDR_SAV 2048
#define NB_TOP_ADDR 5

#pragma GCC diagnostic ignored "-Wunused-parameter"

struct nbRequest_t {
    uint32_t address[MAX_ADDR_SAV];
    unsigned int requestCount[MAX_ADDR_SAV]; 
    int lastIndex;
};

struct nbRequest_t nbRequest;

int getPortArray(char portsArg[], char ports[MAX_PORTS][MAX_SERVICE_NAME]){
    int j = 0;
    char tmp[MAX_SERVICE_NAME] = "";
    for(int i = 0; i < (int)strlen(portsArg) + 1; i++){
        if (portsArg[i] != ',' && portsArg[i] != '\0'){
            strncat(tmp,&portsArg[i],1);
        }
        else{
            strcpy(ports[j], tmp);
            strcpy(tmp, "");
            j++;
        }
    }
    return j;
}   

void analyzeArg(int argc, char* argv[], char ports[MAX_PORTS][MAX_SERVICE_NAME], int* nbPorts){

    int option;

    static struct option long_options[] = {
        {"port", required_argument, NULL, 'p'}
    };
    while((option = getopt_long(argc, argv, "p:", long_options, NULL)) != -1){
        switch(option) {
            case 'p':
                char portsArg[MAX_PORTS*MAX_SERVICE_NAME+MAX_PORTS];
                strcpy(portsArg, optarg);
                portsArg[strlen(optarg)] = '\0';
                *nbPorts = getPortArray(portsArg, ports);                
                break;
                
            default :
                perror("Arguments incorrects: -p [port1,port2,...] / --port [port1,port2,...]");
                exit(EXIT_FAILURE);
        }
    }
}

uint32_t extractSourceAddress(char *packet) {
    // Cast du paquet au type de structure iphdr
    struct iphdr *ipHeader = (struct iphdr *)packet;

    // Conversion de l'adresse source à l'ordre d'octets de l'hôte
    uint32_t sourceAddress = ntohl(ipHeader->saddr);

    return sourceAddress;
}

void addAddress(uint32_t address){
    nbRequest.address[nbRequest.lastIndex] = address;
    nbRequest.requestCount[nbRequest.lastIndex] = 1;
    nbRequest.lastIndex++;
}

void newAddress(uint32_t address){
    for(int i=0; i<nbRequest.lastIndex; i++){
        if(nbRequest.address[i] == address){
            nbRequest.requestCount[i]++;
            return;
        }
    }
    addAddress(address);
}

int isInTop(uint32_t t[], uint32_t x, int len){
    for(int i=0; i<len; i++){
        if(t[i] == x) return 1;
    }
    return 0;
}

void sendTop(){

    unsigned int maxRequest = 0;
    uint32_t topAddress[NB_TOP_ADDR] = {0};
    unsigned int topCount[NB_TOP_ADDR] = {0};

    // for(int i=0; i<NB_TOP_ADDR; i++){
    //     topAddress[i] = 0;
    // }

    for(int rank=0; rank<NB_TOP_ADDR; rank++){
        for(int i=0; i<nbRequest.lastIndex; i++){
            if(!isInTop(topAddress, nbRequest.address[i], rank) && nbRequest.requestCount[i] > maxRequest){
                topAddress[rank] = nbRequest.address[i];
                topCount[rank] = nbRequest.requestCount[i];
                maxRequest = nbRequest.requestCount[i];
            }
        }
        maxRequest = 0;
    }

    printf("Stat : \n");
    for(int i=0; i<NB_TOP_ADDR; i++){ 
        printf("%d: %u.%u.%u.%u nb = %d\n", i+1, ((topAddress[i] >> 24) & 0xFF), ((topAddress[i] >> 16) & 0xFF), ((topAddress[i] >> 8) & 0xFF), (topAddress[i] & 0xFF), topCount[i]);
    }
}

void packet_handler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    //Fonction de traitement du paquet
    printf("Nouveau paquet capturé !\n");
    
    char *ipPacket = (char *)(packet+14);

    uint32_t addrIpSrc = extractSourceAddress(ipPacket);

    newAddress(addrIpSrc);
    sendTop();
}

int main(int argc, char* argv[]){
    
    char ports[MAX_PORTS][MAX_SERVICE_NAME];
    int nbPorts = 2;
    char *device = argv[1];
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program filter;
    char filter_exp[MAX_FILTER_EXP];
    char local_ip[INET_ADDRSTRLEN];

    // Initialisation des sauvegardes d'adresse
    for(int i=0; i<MAX_ADDR_SAV; i++){
        nbRequest.address[i] = 0;
        nbRequest.requestCount[i] = 0;
    }
    nbRequest.lastIndex = 0;

    // Obtenir l'adresse ip local
    printf("Entrez votre adresse ip locale format x.x.x.x\n");
    scanf("%s", local_ip);

    // Gestion du filtre avec les arguments

    snprintf(filter_exp, sizeof(filter_exp), "dst host %s and (port ", local_ip);
    strcpy(ports[0], "80");
    strcpy(ports[1], "443");
    analyzeArg(argc, argv, ports, &nbPorts);
    strcat(filter_exp, ports[0]);
    for(int i=1; i<nbPorts; i++){
        strcat(filter_exp, " or dst port ");
        strcat(filter_exp, ports[i]);
    }
    strcat(filter_exp, ")");
    // Debug info
    printf("Lecture des paquets sur l'interface %s\n", device);
    printf("avec le filtre_exp : %s\n", filter_exp);
    
    handle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    
    if (handle == NULL){
        printf("Impossible d'ouvrir %s - %s\n", device, errbuf);
        return 2;
    }
    
    if (pcap_compile(handle, &filter, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1){
        printf("Mauvais filtre - %s\n", pcap_geterr(handle));
        return 2;
    }
    
    if (pcap_setfilter(handle, &filter) == -1){
        printf("Erreur en mettant le filtre - %s\n", pcap_geterr(handle));
        return 2;
    }
    
    pcap_loop(handle, 0, packet_handler, NULL);
    
    //Fermeture de la capture
    pcap_close(handle);
    
    return 0;
}

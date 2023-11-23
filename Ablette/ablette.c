#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <pcap.h>

#define MAX_SERVICE_NAME 32
#define MAX_PORTS 32

int i=0;

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

// int packetHandler(char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet){
//     //Fonction de traitement du paquet
//     i ++;
//     printf("Utilisateur : %s %d\n", user, i);
//     return 0;
// }

int main(int argc, char* argv[]){
    
    char ports[MAX_PORTS][MAX_SERVICE_NAME];
    int nbPorts = 2;
    char *dev = argv[1];
    char errbuf[PCAP_ERRBUF_SIZE];
    
    strcpy(ports[0], "80");
    strcpy(ports[1], "443");
    
    // pcap_t *handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    // if (handle == NULL) { printf("Impossible d'ouvir l'interface %s : %s\n", dev, errbuf); perror("interface.open"); return -1;}
    
    // //Lancement de la capture
    // // pcap_loop(handle, 0, (void *)packetHandler, NULL);
    
    // packet = pcap_next(handle, &header);
    
    //  /* Décoder l'en-tête Ethernet en convertissant le packet en une
    // struct */
    // struct sniff_ethernet* ethernet = (struct sniff_ethernet *) (packet);
    // /* Si c'est de l'IPv4 */
    // if (ntohs(ethernet->ether_type) == IPv4_ETHERTYPE) {
    //       /* Décoder IPv4 : convertir le paquet en une struct. Ne pas
    // oublier de décaler de SIZE_ETHERNET octets */
    // ip = (struct sniff_ip *) (packet + SIZE_ETHERNET);
    // /* Afficher */
    // printf ("Paquet IPv4 avec le protocole %d\n", ip->ip_p);
    
    // //Fermeture de la capture
    // pcap_close(handle);
    
    analyzeArg(argc, argv, ports, &nbPorts);
    
    
    
    return 0;
}

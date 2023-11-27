#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <pcap.h>
#include <netinet/ip.h>


#define MAX_SERVICE_NAME 32
#define MAX_PORTS 32
#define MAX_FILTER_EXP 1024
#define MAX_ADDR_SAVE 5
#define MAX_ADDR 2048

struct addr_ip_t{
    char *addr[MAX_ADDR];
    int nb_mess[MAX_ADDR];
    int nb_addr;
};

struct addr_ip_t addr_ip;
                            

int i=0;
char top_addr[MAX_ADDR_SAVE][2] = {0}; 

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

int add_addr(char *addr_ip_src){
    // for (int i=0; i<addr_ip.nb_addr; i++){
    //     printf("addr_ip.addr[i] : %s addr_ip_src %s %d\n", addr_ip.addr[i], addr_ip_src, i);
    //     if (strcmp(addr_ip.addr[i],addr_ip_src) == 0){
    //         addr_ip.nb_mess[i]++;
    //         return 0;
    //     }
    // }
        
    if (addr_ip.nb_addr < MAX_ADDR){
        addr_ip.addr[addr_ip.nb_addr] = addr_ip_src; // Attention nb_addr vaut null
        addr_ip.nb_mess[addr_ip.nb_addr] = 1;
        addr_ip.nb_addr++;
        printf("test %d\n", addr_ip.nb_addr);
        return 0;
    }
    else{
        printf("Dépassement de la taille max de sauvegarde d'adresse");
        return -1;
    }
}

void send_top_addr(){
    int max[5] = {0};
    char *addr[5];
    
    for(int i=0; i<5; i++){
        addr[i] = "";
    }
    
    for(int i=0; i<5; i++){
        for(int j=0; j<addr_ip.nb_addr; j++){
            for(int k=0; k<5; k++){
                if(strcmp(addr[k], addr_ip.addr[j]) == 0){
                    break;
                }
                if(addr_ip.nb_mess[j] > max[i]){
                    max[i] = addr_ip.nb_mess[j];
                    addr[i] = addr_ip.addr[j];
                }
            }
        }
    }
}


void packet_handler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet){
    //Fonction de traitement du paquet
    printf("Nouveau paquet capturé !\n");
    // printf("Contenue du paquet capturé en hexadécimal : \n");
    // for (bpf_u_int32 i = 0; i<pkthdr->caplen; ++i){
    //     printf("%02x ", packet[i]);
    //     if ((i+1)%16 == 0 || i == pkthdr->caplen - 1){
    //         printf("\n\n");
    //     }
    // }
    
    struct ip *ip_header = (struct ip*) (packet+14);
    
    char addr_ip_src[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_header->ip_dst), addr_ip_src, INET_ADDRSTRLEN);
    printf("addresse ip source : %s\n", addr_ip_src);
    
    add_addr(addr_ip_src);
    printf("1: %s nb : %d\n", addr_ip.addr[0], addr_ip.nb_mess[0]);
    printf("2: %s nb : %d\n", addr_ip.addr[1], addr_ip.nb_mess[1]);
    printf("3: %s nb : %d\n", addr_ip.addr[2], addr_ip.nb_mess[2]);
    printf("4: %s nb : %d\n", addr_ip.addr[3], addr_ip.nb_mess[3]);
}


int main(int argc, char* argv[]){
    
    char ports[MAX_PORTS][MAX_SERVICE_NAME];
    int nbPorts = 2;
    char *device = argv[1];
    pcap_t *handle;
    char error_buffer[PCAP_ERRBUF_SIZE];
    struct bpf_program filter;
    char filter_exp[MAX_FILTER_EXP] = "port ";
    
    // Initialisation des sauvegardes d'adresse
    for(int i=0; i<MAX_ADDR; i++){
        addr_ip.addr[i] = "";
    }
    addr_ip.nb_addr = 0;
    
    // Gestion du filtre avec les arguments
    strcpy(ports[0], "80");
    strcpy(ports[1], "443");
    analyzeArg(argc, argv, ports, &nbPorts);
    strcat(filter_exp, ports[0]);
    for(int i=1; i<nbPorts; i++){
        strcat(filter_exp, " or port ");
        strcat(filter_exp, ports[i]);
    }
    
    // Debug info
    printf("Lecture des paquets sur l'interface %s\n", device);
    printf("avec le filtre_exp : %s\n", filter_exp);
    
    handle = pcap_open_live(device, BUFSIZ, 1, 1000, error_buffer);
    
    if (handle == NULL){
        printf("Impossible d'ouvrir %s - %s\n", device, error_buffer);
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

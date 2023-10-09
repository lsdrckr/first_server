#ifndef SERVER_H
#define SERVER_H

int serverInit(char *service, int connections);
/* Initialise le server sur le port du service et avec le nombre de connections spécifiées
service : service utilisé parmis la liste des etc/services ex "tcp" => port 15
connections : nombre de connections qui peuvent rester en queue avant d'être traitées
*/

int serverLoop(int sockFd, int (*traitement)(int));
/* Attends une nouvelle connexion puis l'accepte et lance traitement pour le client
sockFd : descripteur du socket
traitement : nom de la fonction de traitement
*/


#endif

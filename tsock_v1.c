// /* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>


// FONCTIONS

// Création message	
void construire_message(char *message, char motif, int lg) {
	int i;
	for (i=0;i<lg;i++){
        message[i] = motif;
    }
}

// Affichage message
void afficher_message(char *message, int lg) {
	int i;
	printf("message construit : ");
	for (i=0;i<lg;i++) {
        printf("%c", message[i]); printf("\n");
    } 
}

// MAIN 

void main (int argc, char **argv)
{
    //Variables 
    
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
	int protocole = -1 ; /* 0= TCP et 1= UDP */

	int sock ;
	struct sockaddr_in adr_local ; //adresse du socket local
	int lg_adr_local = sizeof(adr_local);
	struct sockaddr_in adr_distant ; //adresse du socket distant
	int lg_adr_distant = sizeof(adr_distant);
    int n_port = atoi(argv[argc-1]);
    char *host_name = argv[argc-2];

	char *pmesg = malloc(30*sizeof(char)); //message à envoyer
	int lg_message = 30; //longueur message à envoyer, 30 par défaut
	int mesg_emis ;
    int mesg_recu ;
	int lg_max ; //espace réservé pour stocker message reçu



    //Gestion arguments à l'appel de fonction
    while ((c = getopt(argc, argv, "pn:su")) != -1) {
		switch (c) {
		case 'p':
			if (source == 1) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1) ;
			}
			source = 1;
			break;

		case 'n':
			nb_message = atoi(optarg);
                printf("Nb de message : %d/n", nb_message);
			break;

		case 'u':
			protocole = 1;


		default:
			printf("usage: cmd [-p|-s][-n ##]\n");
			break;
		}
	}


    //Gestion source : 0=puits, 1=source
	if (source == -1) {
		printf("usage: cmd [-p|-s][-n ##]\n");
		exit(1) ;
	}

	if (source == 1){
		printf("on est dans la source\n");
    }
	else {
        printf("on est dans le puits\n");
    }

    // Gestion nb de messages
	if (nb_message != -1) {
		if (source == 1) {
            printf("nb de tampons à envoyer : %d\n", nb_message);
        }
		else {
            printf("nb de tampons à recevoir : %d\n", nb_message);
        }
	} else {
		if (source == 1) {
			nb_message = 10 ;
			printf("nb de tampons à envoyer = 10 par défaut\n");
		} 
        else {
            printf("nb de tampons à envoyer = infini\n");
        }
	}

    //Gestion protocole : 0= TCP et 1= UDP
	if (protocole ==-1){
		printf("erreur de protocole\n");
	}

    //UDP
	if (protocole ==1) {

        //Côté source
        if (source == 1){

            printf("lg_message=%i, port=%i, nb_envois=?, TP=UDP, dest=%s \n",lg_message,n_port,host_name);

            //Création de socket
            if ((sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1) {	
			    printf("echec de création socket/n");
			    exit(1);
            }

            // Construction @ local
		    memset((char*)&adr_local, 0, sizeof(adr_local)); //reset 
		    adr_local.sin_family = AF_INET;
		    adr_local.sin_port = n_port ; //htons(atoi(argv[argc-1]));
		    adr_local.sin_addr.s_addr = INADDR_ANY ;

            // Association @ à socket
		    if(bind(sock,(struct sockaddr *)&adr_local,lg_adr_local) ==-1) {
			    printf("échec du bind/n");
			    exit(1);
            }

            //Construction message & envoi / réception
            construire_message(pmesg,'a',lg_message);
		    mesg_emis = sendto(sock,pmesg,sizeof(pmesg),0,(struct sockaddr *)&adr_distant,lg_adr_distant);
            if (mesg_emis == -1) {
                printf("échec envoi message\n");
                exit(1);
            }
            afficher_message(pmesg,lg_message);
        }
        //Côté puit
        else{

            // Construction @ distant
		    memset((char*)&adr_distant, 0, sizeof(adr_distant)); //reset 
		    adr_distant.sin_family = AF_INET;
		    adr_distant.sin_port = n_port;
		    adr_distant.sin_addr.s_addr = INADDR_ANY ; 

            // Réception message
            mesg_recu = recvfrom(sock,pmesg,lg_max,0,(struct sockaddr *)&adr_local,&lg_adr_local);
            if (mesg_recu == -1) {
                printf("échec réception message\n");
                exit(1);
            }
            afficher_message(pmesg,lg_message);

		}

      

        
	}

}

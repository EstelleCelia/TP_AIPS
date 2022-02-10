/* librairie standard ... */
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

void main (int argc, char **argv)
{
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
	int protocole = -1 ; /* 0= TCP et 1= UDP */
	int sock ;
	struct sockaddr_in adr_local ;
	int lg_adr_local = sizeof(adr_local);
	struct sockaddr_in adr_distant ;
	int lg_adr_distant = sizeof(adr_distant);
	char *message = malloc(40*sizeof(char));
	int lg_message = 30;
	int lg_emis ;
	int lg_max ;
	char *pmesg ;
	struct sockaddr *padr_em ;
	int *plg_adr_em ;
	int mesg_recu ;
	char messagetest[11] = "abcdefghij";
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
			break;

		case 'u':
			protocole = 1;


		default:
			printf("usage: cmd [-p|-s][-n ##]\n");
			break;
		}
	}


	void construire_message(char *message, char motif, int lg) {
		int i;
		for (i=0;i<lg;i++) message[i] = motif;}
	void afficher_message(char *message, int lg) {
		int i;
		printf("message construit : ");
		for (i=0;i<lg;i++) printf("%c", message[i]); printf("\n");}


	if (protocole ==-1){
		printf("erreur de protocole");
	}

	if (protocole ==1) {
		if ((sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1) {	
			printf("echec de création socket/n");
			exit(1);}

		memset((char*)&adr_local, 0, sizeof(adr_local)); /* reset */
		adr_local.sin_family = AF_INET;
		adr_local.sin_port = htons(atoi(argv[argc-1]));
		adr_local.sin_addr.s_addr = INADDR_ANY ;

		if(bind(sock,(struct sockaddr *)&adr_local,lg_adr_local) ==-1) {
			printf("échec du bind/n");
			exit(1); }
	}


	if (source == -1) {
		printf("usage: cmd [-p|-s][-n ##]\n");
		exit(1) ;
	}

	if (source == 1)
		printf("on est dans la source\n");
		if (protocole==1){
			// construire_message(message,'a',25);
			lg_emis = sendto(sock,message,lg_message,0,(struct sockaddr *)&adr_distant,lg_adr_distant);
			printf("lg emis :%d \n", lg_emis);
		}
	else
		printf("on est dans le puits\n");
		if (protocole==1){
			mesg_recu = recvfrom(sock,pmesg,lg_max,0,padr_em,plg_adr_em);
		}




	if (nb_message != -1) {
		if (source == 1)
			printf("nb de tampons à envoyer : %d\n", nb_message);
		else
			printf("nb de tampons à recevoir : %d\n", nb_message);
	} else {
		if (source == 1) {
			nb_message = 10 ;
			printf("nb de tampons à envoyer = 10 par défaut\n");
		} else
		printf("nb de tampons à envoyer = infini\n");

	}
}


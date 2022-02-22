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
#include <string.h>

// FONCTIONS

// Création message
void construire_message(char *message, char motif, int lg)
{
    int i;
    for (i = 0; i < lg; i++)
    {
        message[i] = motif;
    }
}

// Affichage message
void afficher_message(char *message, int lg)
{
    int i;
    printf("message construit : ");
    for (i = 0; i < lg; i++)
    {
        printf("%c", message[i]);
        printf("\n");
    }
}

// MAIN

int main(int argc, char **argv)
{
    // Variables

    int c;
    extern char *optarg;
    extern int optind;
    int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
    int source = -1;     /* 0=puits, 1=source */
    int protocole = -1;  /* 0= TCP et 1= UDP */

    int sock;
    struct sockaddr_in adr_local; // adresse du socket local
    unsigned int lg_adr_local = sizeof(adr_local);
    struct sockaddr_in adr_distant; // adresse du socket distant
    unsigned int lg_adr_distant = sizeof(adr_distant);
    struct hostent *hp;
    struct sockaddr *padr_em;
    int *plg_adr_em;
    int n_port = atoi(argv[argc - 1]);
    char *host_name = argv[argc - 2];

    char *mesg_envoye;
    char *mesg_recu;
    mesg_envoye = (char *)malloc(30 * sizeof(char)); // message à envoyer
    mesg_recu = (char *)malloc(30 * sizeof(char));
    int lg_message = 30; // longueur message à envoyer, 30 par défaut

    // Gestion arguments à l'appel de fonction
    while ((c = getopt(argc, argv, "pn:su")) != -1)
    {
        switch (c)
        {
        case 'p':
            if (source == 1)
            {
                printf("usage: cmd [-p|-s][-n ##]\n");
                exit(1);
            }
            source = 0;
            break;

        case 's':
            if (source == 0)
            {
                printf("usage: cmd [-p|-s][-n ##]\n");
                exit(1);
            }
            source = 1;
            break;

        case 'n':
            nb_message = atoi(optarg);
            printf("Nb de message : %d\n", nb_message);
            break;

        case 'u':
            protocole = 1;

        default:
            printf("usage: cmd [-p|-s][-n ##]\n");
            break;
        }
    }

    // Gestion source : 0=puits, 1=source
    if (source == -1)
    {
        printf("usage: cmd [-p|-s][-n ##]\n");
        exit(1);
    }

    if (source == 1)
    {
        printf("on est dans la source\n");
    }
    else
    {
        printf("on est dans le puits\n");
    }

    // Gestion nb de messages
    if (nb_message != -1)
    {
        if (source == 1)
        {
            printf("nb de tampons à envoyer : %d\n", nb_message);
        }
        else
        {
            printf("nb de tampons à recevoir : %d\n", nb_message);
        }
    }
    else
    {
        if (source == 1)
        {
            nb_message = 10;
            printf("nb de tampons à envoyer = 10 par défaut\n");
        }
        else
        {
            printf("nb de tampons à envoyer = infini\n");
        }
    }

    // Gestion protocole : 0= TCP et 1= UDP
    if (protocole == -1)
    {
        printf("erreur de protocole\n");
    }

    // UDP
    if (protocole == 1)
    {

        // Côté SOURCE / emetteur
        if (source == 1)
        {

            printf("lg_message=%i, port=%i, nb_envois=?, TP=UDP, dest=%s \n", lg_message, n_port, host_name);

            // Création de socket
            if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            {
                printf("echec de création socket\n");
                exit(1);
            }

            // Construction @ distant
            memset((char *)&adr_distant, 0, sizeof(adr_distant)); // reset
            adr_distant.sin_family = AF_INET;
            adr_distant.sin_port = n_port; // htons(atoi(argv[argc-1]));
            // adr_local.sin_addr.s_addr = INADDR_ANY;

            /*
            // Association @ à socket
            if (bind(sock, (struct sockaddr *)&adr_local, lg_adr_local) == -1)
            {
                printf("échec du bind\n");
                exit(1);
            }*/

            // Construction message
            construire_message(mesg_envoye, 'a', lg_message);

            // Envoi message
            if ((sendto(sock, mesg_envoye, sizeof(mesg_envoye), 0, (struct sockaddr *)&adr_distant, lg_adr_distant)) == -1)
            {
                printf("échec envoi message\n");
                exit(1);
            }

            afficher_message(mesg_envoye, lg_message);

            // destruction socket
            if (close(sock) == -1)
            {
                printf("Erreur destruction socket\n");
                exit(1);
            }
        }

        // Côté PUIT / recepteur
        else
        {

            printf("lg_message=%i, port=%i, nb_receptions=?, TP=UDP \n", lg_message, n_port);

            // Création de socket
            if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            {
                printf("echec de création socket\n");
                exit(1);
            }

            // Construction @ local
            memset((char *)&adr_local, 0, sizeof(adr_local)); // reset
            adr_local.sin_family = AF_INET;
            adr_local.sin_port = n_port;
            adr_local.sin_addr.s_addr = INADDR_ANY;

            /*// affectation adresse IP
            if ((hp = gethostbyname(host_name)) == NULL)
            {
                printf("échec gethostbyname\n");
                exit(1);
            }
            memcpy((char *)&(adr_distant.sin_addr.s_addr), hp->h_addr, hp->h_length);*/

            // Association @ à socket
            if (bind(sock, (struct sockaddr *)&adr_local, lg_adr_local) == -1)
            {
                printf("échec du bind\n");
                exit(1);
            }

            // Réception message
            if ((recvfrom(sock, mesg_recu, lg_message, 0, (struct sockaddr *)&padr_em, plg_adr_em)) == -1)
            {
                printf("échec réception message\n");
                exit(1);
            }

            printf("après réception message \n");
            afficher_message(mesg_recu, lg_message);

            // destruction socket
            if (close(sock) == -1)
            {
                printf("Erreur destruction socket\n");
                exit(1);
            }
        }
    }
    return 0;
}

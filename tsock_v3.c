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

void construire_message(char mesg[], char lettre, int lg, int num)
{
    num++;
    int numero = num;
    int nb_chiffres = 1;

    while (numero >= 10)
    {
        numero /= 10;
        nb_chiffres += 1;
    }

    for (int i = 0; i < (5 - nb_chiffres); i++)
    {
        mesg[i] = '-';
    }

    int num1 = 10;
    int num2 = 1;
    int num_envoi;

    for (int i = 4; i >= (5 - nb_chiffres); i--)
    {
        num_envoi = (num % num1) / num2;
        mesg[i] = (char)(48 + num_envoi);
        num1 = num1 * 10;
        num2 = num2 * 10;
    }

    for (int i = 5; i < lg; i++)
    {
        mesg[i] = lettre;
    }
}

//Affichage message source
void afficher_message_source(char *mesg, int lg, int num)
{
    int i;
    printf("SOURCE : Envoi n°%i (%i) ", num, lg);
    printf("[");
    for (i = 0; i < lg; i++)
    {
        printf("%c", mesg[i]);
    }
    printf("]\n");
}

//Affichage message puit
void afficher_message_puit(char *mesg, int lg, int num)
{
    int i;
    printf("PUITS : Reception n°%i (%i) ", num, lg);
    printf("[");
    for (i = 0; i < lg; i++)
    {
        printf("%c", mesg[i]);
    }
    printf("]\n");
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
    int protocole = 0;  /* 0= TCP et 1= UDP */

    int sock, sock_bis;
    struct sockaddr_in adr_local; // adresse du socket local
    unsigned int lg_adr_local = sizeof(adr_local);
    struct sockaddr_in adr_distant; // adresse du socket distant
    unsigned int lg_adr_distant = sizeof(adr_distant);
    struct hostent *hp;
    struct sockaddr *padr_em;
    struct sockaddr* adr_client;
    int lg_adr_client;
    int lg_mesg_recu;
    int lg_adr_em = sizeof(struct sockaddr_in);
    int n_port = atoi(argv[argc - 1]);
    char *host_name = argv[argc - 2];

    char *mesg_envoye;
    char *mesg_recu;
    mesg_envoye = (char *)malloc(30 * sizeof(char)+1); // message à envoyer
    mesg_recu = (char *)malloc(30 * sizeof(char)+1);
    int lg_message = 30; // longueur message à envoyer, 30 par défaut
    char alphabet[] = "abcdefghijklmnopqrstuvwxyz";

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

            case 'l':
                lg_message = atoi(optarg);

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

            printf("lg_message=%i, port=%i, nb_envois=%i, TP=UDP, dest=%s \n", lg_message, n_port, nb_message, host_name);

            // Création de socket
            if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            {
                printf("echec de création socket\n");
                exit(1);
            }

            // Construction @ distant
            memset((char *)&adr_distant, 0, sizeof(adr_distant)); // reset
            adr_distant.sin_family = AF_INET;
            adr_distant.sin_port = htons(n_port); // htons(atoi(argv[argc-1]));
            // adr_local.sin_addr.s_addr = INADDR_ANY;


            if((hp = gethostbyname(host_name)) == NULL){
                printf("Erreur lors de l'attributon de l'adresse via gethostbyname");
                exit(1);
            }

            memcpy((char*)&(adr_distant.sin_addr.s_addr),
                   hp->h_addr,
                   hp->h_length);


            // Construction message

            for (int i=0 ; i<nb_message; i++){

                // Construction message
                construire_message(mesg_envoye, alphabet[i%26], lg_message, i);

                // Envoi message
                if ((sendto(sock, mesg_envoye, sizeof(mesg_envoye), 0, (struct sockaddr *)&adr_distant, lg_adr_distant)) == -1)
                {
                    printf("échec envoi message\n");
                    exit(1);
                }

                //Affichage message
                afficher_message_source(mesg_envoye, lg_message, i+1);

            }

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


            // Association @ à socket
            if (bind(sock, (struct sockaddr *)&adr_local, lg_adr_local) == -1)
            {
                printf("échec du bind\n");
                exit(1);
            }

            // Réception message

            if (nb_message != -1)
            {
                printf("PUITS : lg_mesg-lu=%i, port=%i, nb_receptions= %i, TP=udp \n",lg_message, n_port, nb_message);
                for (int i=0; i < nb_message; i++)
                {
                    if ( recvfrom(sock, mesg_recu, sizeof(mesg_recu), 0, (struct sockaddr *)&padr_em, & lg_adr_em) == -1 ){
                        printf("échec réception message\n");
                        exit(1);
                    }
                    else afficher_message_puit(mesg_recu, sizeof(mesg_recu), i);
                }
            }
            else
            {
                int i=0;
                printf("PUITS : lg_mesg-lu=%i, port=%i, nb_receptions= infini, TP=udp \n",lg_message, n_port);
                while(1)
                {
                    recvfrom(sock, mesg_recu,lg_message, 0, (struct sockaddr *)&padr_em, & lg_adr_em);
                    afficher_message_puit(mesg_recu, lg_message, i+1);
                    i++;
                }
            }

            // destruction socket
            if (close(sock) == -1)
            {
                printf("Erreur destruction socket\n");
                exit(1);
            }
        }
    }

    else // TCP
    {
        // coté source
        if(source == 1)
        {
            printf("SOURCE : lg_mesg-emis=%i, port=%i, nb_envois= %i, TP=TCP, dest=%s \n",lg_message, n_port, nb_message, host_name);

            //Création de socket
            if ((sock = socket(AF_INET, SOCK_STREAM, 0))==-1)
            {
                printf("Erreur création socket\n");
                exit(1);
            }


            //Construction @ distant
            memset((char*)&adr_distant, 0, sizeof(adr_distant));
            adr_distant.sin_family = AF_INET;
            adr_distant.sin_port = n_port;

            //Affectation adresse IP
            if ((hp=gethostbyname(host_name))==NULL)
            {
                printf("Erreur gethostbyname\n");
                exit(1);
            }
            memcpy((char*)&(adr_distant.sin_addr.s_addr),
                   hp->h_addr,
                   hp->h_length);

            //connexion
            if ((connect(sock,(struct sockaddr*)& adr_distant, sizeof(adr_distant))) == -1)
            {
                printf("Erreur connection\n");
                exit(1);
            }

            // Construction message

            for (int i=0 ; i<nb_message; i++){

                // Construction message
                construire_message(mesg_envoye, alphabet[i%26], lg_message, i);

                // Envoi message
                if (send(sock, mesg_envoye, sizeof(mesg_envoye), 0) == -1)
                {
                    printf("échec envoi message\n");
                    exit(1);
                }

                //Affichage message
                afficher_message_source(mesg_envoye, lg_message, i+1);

            }

            printf("SOURCE : fin\n");

            //destruction socket
            if (close(sock)==-1)
            {
                printf("Erreur destruction socket\n");
                exit(1);
            }

        }

        else
        {
            //coté PUIT

            //création socket
            if ((sock = socket(AF_INET, SOCK_STREAM, 0))==-1)
            {
                printf("Erreur création socket\n");
                exit(1);
            }

            //Construction @ locale
            memset((char*)& adr_local, 0, sizeof(adr_local));
            adr_local.sin_family = AF_INET;
            adr_local.sin_port = n_port;
            adr_local.sin_addr.s_addr = INADDR_ANY;

            //bind
            if (bind(sock, (struct sockaddr*)&adr_local, lg_adr_local)==-1)
            {
                printf("Erreur bind\n");
                exit(1);
            }

            //listen
            int max_co=5;
            listen(sock,max_co);

            //acceptation connexion
            if((sock_bis = accept(sock,(struct sockaddr *) &adr_client, &lg_adr_client))<0)
            {
                printf("Erreur accept\n");
                exit(1);
            }

            char message_recu[lg_message+1];
            int i;

            //réception des messages
            if (nb_message != -1)
            {
                printf("PUITS : lg_mesg-lu=%i, port=%i, nb_receptions= %i, TP=TCP \n",lg_message, n_port, nb_message);
                for (i=0; i < nb_message; i++)
                {
                    if((lg_mesg_recu=read(sock_bis,message_recu,lg_message))<0)
                    {
                        printf("Erreur read\n");
                        exit(1);
                    }
                    afficher_message_puit(message_recu, lg_mesg_recu,i+1);
                }
            }
            else
            {
                int i=1;
                printf("PUITS : lg_mesg-lu=%i, port=%i, nb_receptions= infini, TP=TCP \n",lg_message, n_port);
                while ((lg_mesg_recu=read(sock_bis,message_recu,lg_message))>0)
                {
                    afficher_message_puit(message_recu, lg_message,i);
                    i++;
                }
            }

            //destruction socket
            if (close(sock)==-1)
            {
                printf("Erreur destruction socket\n");
                exit(1);
            }
        }
    }


    return 0;


}
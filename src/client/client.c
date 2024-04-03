#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAXBUFLEN 100
#define SERVERPORT "4950" // the port users will be connecting to
#define TRUE 1
#define FALSE 0
struct Music
{
    int id;
    char titulo[100];
    char interprete[100];
    char idioma[100];
    char tipo[100];
    int ano;
    char refrao[];
};

char *intToChar(int num)
{
    char *str = malloc(10 * sizeof(char));
    sprintf(str, "%d", num);
    return str;
}

int sendData(char *ip, char *data)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(ip, SERVERPORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }

    if ((numbytes = sendto(sockfd, data, strlen(data), 0,
                           p->ai_addr, p->ai_addrlen)) == -1)
    {
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);

    printf("talker: sent %d bytes to %s\n", numbytes, ip);
    close(sockfd);

    return 0;
}

char *login()
{
    // select 1 if you are admin and 2 if you are a user
    printf("Select an option\n");
    printf("1. Admin\n");
    printf("2. User\n");
    int option;
    scanf("%d", &option);
    if (option == 1)
    {
        return "admin";
    }
    else
    {
        return "user";
    }
}

int isAdmin(char *userSecret)
{
    if (strcmp(userSecret, "admin") == 0)
    {
        return TRUE;
    }
    return FALSE;
}

int registerSong(char *ip)
{
    printf("Register song\n");
    printf("Enter song name:\n");
    char songName[100];
    getchar();
    fgets(songName, sizeof(songName), stdin);
    // Remove the trailing newline character
    songName[strcspn(songName, "\n")] = 0;
    printf("Enter song year:\n");
    int songYear;
    scanf("%d", &songYear);
    printf("Enter song language:\n");
    char songLanguage[100];
    scanf("%s", songLanguage);
    printf("Enter song type:\n");
    char songType[100];
    getchar();
    fgets(songType, sizeof(songType), stdin);
    songType[strcspn(songType, "\n")] = 0;
    printf("Enter song artist:\n");
    char songArtist[100];
    getchar();
    fgets(songArtist, sizeof(songArtist), stdin);
    songArtist[strcspn(songArtist, "\n")] = 0;
    printf("Enter song chorus:\n");
    char songChorus[100];
    getchar();
    fgets(songChorus, sizeof(songChorus), stdin);
    songChorus[strcspn(songChorus, "\n")] = 0;
    char encoded[MAXBUFLEN];
    strcpy(encoded, "6");
    strcat(encoded, "|");
    strcat(encoded, songName);
    strcat(encoded, "|");
    strcat(encoded, intToChar(songYear));
    strcat(encoded, "|");
    strcat(encoded, songLanguage);
    strcat(encoded, "|");
    strcat(encoded, songType);
    strcat(encoded, "|");
    strcat(encoded, songArtist);
    strcat(encoded, "|");
    strcat(encoded, songChorus);
    sendData(ip, encoded);
    // TODO: RECEBER INFORMAÇÕES DO SERVIDOR E IMPRIMIR
}

int removeSong(char *ip)
{
    printf("Remove song\n");
    printf("Enter song id: ");
    int songId;
    scanf("%d", &songId);
    // concat songId with operation code in a string before sending
    char encoded[MAXBUFLEN];
    strcpy(encoded, "7");
    strcat(encoded, "|");
    strcat(encoded, intToChar(songId));
    sendData(ip, encoded);
    // TODO: RECEBER INFORMAÇÕES DO SERVIDOR E IMPRIMIR
}

int listSongsByYear(char *ip)
{
    printf("List songs by year\n");
    printf("Enter year: ");
    int ano;
    scanf("%d", &ano);
    // concat ano with operation code in a string before sending
    char encoded[MAXBUFLEN];
    strcpy(encoded, "1");
    strcat(encoded, "|");
    strcat(encoded, intToChar(ano));
    sendData(ip, encoded);
    // TODO: RECEBER INFORMAÇÕES DO SERVIDOR E IMPRIMIR
}

int listSongsByLanguageAndYear(char *ip)
{
    printf("List songs by language and year\n");
    printf("Enter language: ");
    char idioma[100];
    scanf("%s", idioma);
    printf("Enter year: ");
    int ano;
    scanf("%d", &ano);
    char encoded[MAXBUFLEN];
    strcpy(encoded, "1");
    strcat(encoded, "|");
    strcat(encoded, intToChar(ano));
    strcat(encoded, "|");
    strcat(encoded, idioma);
    sendData(ip, encoded);
    // TODO: RECEBER INFORMAÇÕES DO SERVIDOR E IMPRIMIR
}

int listSongsByType(char *ip)
{
    printf("List songs by type\n");
    printf("Enter type: ");
    char type[100];
    scanf("%s", type);
    // concat type with operation code in a string before sending
    char encoded[MAXBUFLEN];
    strcpy(encoded, "3");
    strcat(encoded, "|");
    strcat(encoded, type);
    sendData(ip, encoded);
    // TODO: RECEBER INFORMAÇÕES DO SERVIDOR E IMPRIMIR
}

int listSongInformation(char *ip)
{
    printf("List song information\n");
    printf("Enter song id: ");
    int songId;
    scanf("%d", &songId);
    // concat songId with operation code in a string before sending
    char encoded[MAXBUFLEN];
    strcpy(encoded, "4");
    strcat(encoded, "|");
    strcat(encoded, intToChar(songId));
    sendData(ip, encoded);
    // TODO: RECEBER INFORMAÇÕES DO SERVIDOR E IMPRIMIR
}

int listAllSongInformation(char *ip)
{
    printf("Listing all song information\n");
    // concat operation code in a string before sending
    char encoded[MAXBUFLEN];
    strcpy(encoded, "5");
    sendData(ip, encoded);
    // TODO: RECEBER INFORMAÇÕES DO SERVIDOR E IMPRIMIR
}

int main(int argc, char *argv[])
{
    char *userSecret = "";
    int userOption;
    if (argc != 2)
    {
        fprintf(stderr, "usage: talker hostname\n");
        exit(1);
    }
    while (TRUE)
    {
        /// show options for user login or register
        if (strcmp(userSecret, "") == 0)
        {
            printf("1. Login\n");

            printf("Enter your choice: ");
            scanf("%d", &userOption);
            switch (userOption)
            {
            case 1:
                userSecret = login();
                break;
            default:
                printf("Invalid option\n");
                break;
            }
        }
        else
        {
            printf("User options\n");
            int operation;
            // show any user music options
            printf("1. List songs by year\n");
            printf("2. List songs by language and year\n");
            printf("3. List songs by type\n");
            printf("4. List song information\n");
            printf("5. List all song information\n");
            if (isAdmin(userSecret))
            {
                // show admin music options
                printf("6. Register song\n");
                printf("7. Remove song\n");
            }
            printf("Enter your choice: ");
            scanf("%d", &operation);
            switch (operation)
            {
            case 1:
                listSongsByYear(argv[1]);
                break;
            case 2:
                listSongsByLanguageAndYear(argv[1]);
                break;
            case 3:
                listSongsByType(argv[1]);
                break;
            case 4:
                listSongInformation(argv[1]);
                break;
            case 5:
                listAllSongInformation(argv[1]);
                break;
            case 6:
                registerSong(argv[1]);
                break;
            case 7:
                removeSong(argv[1]);
                break;
            default:
                printf("Invalid option\n");
                break;
            }
        }

        // sendData(argv[1], argv[2]);
    }
}
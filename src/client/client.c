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

#define SERVERPORT "4950" // the port users will be connecting to
#define TRUE 1
#define FALSE 0
struct user
{
    char *username;
    char *password;
    int operation;
};

typedef struct user user;

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

char *login(char *hostname)
{
    printf("Login\n");
    printf("Enter your username: ");
    char username[100];
    scanf("%s", username);
    printf("Enter your password: ");
    char password[100];
    scanf("%s", password);
    user u = {username, password, operation : 0};
    sendData(hostname, "hello");
    // TODO: receive data from server with user secret
    return "userSecret";
}

int isAdmin(char *userSecret)
{
    // TODO check user secret and tell if is an admin or not
    return TRUE;
}

int registerSong(char *userSecret)
{
    printf("Register song\n");
    printf("Enter song name: ");
    char songName[100];
    scanf("%s", songName);
    printf("Enter song year: ");
    int songYear;
    scanf("%d", &songYear);
    printf("Enter song language: ");
    char songLanguage[100];
    scanf("%s", songLanguage);
    printf("Enter song type: ");
    char songType[100];
    scanf("%s", songType);
    printf("Enter song artist: ");
    char songArtist[100];
    scanf("%s", songArtist);
    //TODO: send data to server and receive song id in response
}

int removeSong(char *userSecret)
{
    printf("Remove song\n");
    printf("Enter song id: ");
    int songId;
    scanf("%d", &songId);
    //TODO: send data to server and receive confirmation in response
}

int listSongsByYear(char *userSecret)
{
    printf("List songs by year\n");
    printf("Enter year: ");
    int year;
    scanf("%d", &year);
    //TODO: send data to server and receive list of songs in response
    //print list of songs

}

int listSongsByLanguageAndYear(char *userSecret)
{
    printf("List songs by language and year\n");
    printf("Enter language: ");
    char language[100];
    scanf("%s", language);
    printf("Enter year: ");
    int year;
    scanf("%d", &year);
    //TODO: send data to server and receive list of songs in response
    //print list of songs

}

int listSongsByType(char *userSecret)
{
    printf("List songs by type\n");
    printf("Enter type: ");
    char type[100];
    scanf("%s", type);
    //TODO: send data to server and receive list of songs in response
    //print list of songs
}

int listSongInformation(char *userSecret)
{
    printf("List song information\n");
    printf("Enter song id: ");
    int songId;
    scanf("%d", &songId);
    //TODO: send data to server and receive song information in response
    //print song information
    

}

int listAllSongInformation(char *userSecret)
{
    printf("Listing all song information\n");
    //TODO: send data to server and receive list of songs in response
    //print list of songs

}

int main(int argc, char *argv[])
{
    char *userSecret="";
    int userOption;
    if (argc != 2)
    {
        fprintf(stderr, "usage: talker hostname\n");
        exit(1);
    }
    while (TRUE)
    {
        /// show options for user login or register
        if (strcmp(userSecret, "")==0)
        {
            printf("1. Login\n");
            printf("2. Register (WIP)\n");

            printf("Enter your choice: ");
            scanf("%d", &userOption);
            switch (userOption)
            {
            case 1:
                userSecret = login(argv[1]);
                break;
            // case 2:
            //     printf("Register\n");
            //     break;
            default:
                printf("Invalid option\n");
                break;
            }
        }
        else
        {
            printf("User options\n");
            int operation;
            //show any user music options
            printf("1. List songs by year\n");
            printf("2. List songs by language and year\n");
            printf("3. List songs by type\n");
            printf("4. List song information\n");
            printf("5. List all song information\n");
            if (isAdmin(userSecret))
            {
                //show admin music options
                printf("6. Register song\n");
                printf("7. Remove song\n");
            }
            printf("Enter your choice: ");
            scanf("%d", &operation);
            switch (operation)
            {
            case 1:
                listSongsByYear(userSecret);
                break;
            case 2:
                listSongsByLanguageAndYear(userSecret);
                break;
            case 3:
                listSongsByType(userSecret);
                break;
            case 4:
                listSongInformation(userSecret);
                break;
            case 5:
                listAllSongInformation(userSecret);
                break;
            case 6:
                registerSong(userSecret);
                break;
            case 7:
                removeSong(userSecret);
                break;
            default:
                printf("Invalid option\n");
                break;
            }
        }

        // sendData(argv[1], argv[2]);
    }
}
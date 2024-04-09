#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define MAXBUFLEN 1024
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
    char refrao[256];
};

char *intToChar(int num)
{
    char *str = malloc(100 * sizeof(char));
    sprintf(str, "%d", num);
    return str;
}

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

int sendData(int sockfd, char *data)
{
    int oldLen = strlen(data);
    char *oldLenC = intToChar(oldLen);
    int Newlen = oldLen+ strlen(oldLenC)+1;
    char *newLenC = intToChar(Newlen);
    if(strlen(newLenC)>strlen(oldLenC)){
        Newlen +=1;
    }
    newLenC = intToChar(Newlen);
    strcat(newLenC, "|");
    strcat(newLenC,data);
    int newLen = strlen(newLenC);
    int *len = &newLen;
    sendall(sockfd,newLenC,len);
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

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void *print_result(int sockfd)
{
    char buf[MAXBUFLEN];
    int numbytes;
    if ((numbytes = recv(sockfd, buf, MAXBUFLEN - 1, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    printf("%s\n", buf);
}

int registerSong(int sockfd)
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
    //getchar();
    fgets(songArtist, sizeof(songArtist), stdin);
    songArtist[strcspn(songArtist, "\n")] = 0;
    printf("Enter song chorus:\n");
    char songChorus[100];
    //getchar();
    fgets(songChorus, sizeof(songChorus), stdin);
    songChorus[strcspn(songChorus, "\n")] = 0;
    char encoded[MAXBUFLEN];
    strcpy(encoded, "6");
    strcat(encoded, "|");
    strcat(encoded, songName);
    strcat(encoded, "|");
    strcat(encoded, songArtist);
    strcat(encoded, "|");
    strcat(encoded, songLanguage);
    strcat(encoded, "|");
    strcat(encoded, songType);
    strcat(encoded, "|");
    strcat(encoded, songChorus);
    strcat(encoded, "|");
    strcat(encoded, intToChar(songYear));
    sendData(sockfd, encoded);
    printf("==============\n\n");
    print_result(sockfd);
    printf("==============\n");
}

int removeSong(int sockfd)
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
    sendData(sockfd, encoded);
    printf("==============\n\n");
    print_result(sockfd);
    printf("==============\n");
}

int listSongsByYear(int sockfd)
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
    sendData(sockfd, encoded);
    printf("\nAs musicas do ano de %d são:\n", ano);
    printf("==============\n\n");
    print_result(sockfd);
    printf("==============\n");
}

int listSongsByLanguageAndYear(int sockfd)
{
    printf("List songs by language and year\n");
    printf("Enter language: \n");
    char idioma[100];
    scanf("%s", idioma);
    printf("Enter year: \n");
    int ano;
    scanf("%d", &ano);
    char encoded[MAXBUFLEN];
    strcpy(encoded, "2");
    strcat(encoded, "|");
    strcat(encoded, idioma);
    strcat(encoded, "|");
    strcat(encoded, intToChar(ano));
    sendData(sockfd, encoded);
    printf("\nAs musicas do ano de %d em %s são:\n", ano, idioma);
    printf("==============\n\n");
    print_result(sockfd);
    printf("==============\n");
}

int listSongsByType(int sockfd)
{
    printf("List songs by type\n");
    printf("Enter type:\n");
    char type[100];
    getchar();
    fgets(type, sizeof(type), stdin);
    type[strcspn(type, "\n")] = 0;
    // concat type with operation code in a string before sending
    char encoded[MAXBUFLEN];
    strcpy(encoded, "3");
    strcat(encoded, "|");
    strcat(encoded, type);
    sendData(sockfd, encoded);
    printf("\nAs musicas do ano do genero %s são:\n", type);
    printf("==============\n\n");
    print_result(sockfd);
    printf("==============\n");
}

int listSongInformation(int sockfd)
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
    sendData(sockfd, encoded);
    printf("\nAs informações da musica de ID %d são:\n", songId);
    printf("==============\n\n");
    print_result(sockfd);
    printf("==============\n");
}

int listAllSongInformation(int sockfd)
{
    printf("Listing all song information\n");
    // concat operation code in a string before sending
    char encoded[MAXBUFLEN];
    strcpy(encoded, "5");
    sendData(sockfd, encoded);
    printf("\n informacoes de todas as musicas são:\n");
    printf("==============\n\n");
    print_result(sockfd);
    printf("==============\n");
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
	char buf[MAXBUFLEN];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo);

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
            printf("5. List all songs information\n");
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
                listSongsByYear(sockfd);
                break;
            case 2:
                listSongsByLanguageAndYear(sockfd);
                break;
            case 3:
                listSongsByType(sockfd);
                break;
            case 4:
                listSongInformation(sockfd);
                break;
            case 5:
                listAllSongInformation(sockfd);
                break;
            case 6:
                registerSong(sockfd);
                break;
            case 7:
                removeSong(sockfd);
                break;
            default:
                printf("Invalid option\n");
                break;
            }
        }
    }
}
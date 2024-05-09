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
#include <signal.h>
#include <sys/wait.h>
#include <math.h> // Adicionado para log10


#define MAX_SONGS 100
#define CLIENTPORT "4950"
#define CHUNK_SIZE 1024

#define MYPORT "4950" // the port users will be connecting to

#define MAXBUFLEN 1024

#define BACKLOG 10 // how many pending connections queue will hold
#define CLIENTPORT "4950" // Porta do cliente

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
    int len = strlen(data);
    sendall(sockfd,data,&len);
}

struct Music {
    int id;
    char titulo[100];
    char interprete[100];
    char idioma[100];
    char tipo[100];
    char refrao[256]; // Novo campo adicionado para o refrão
    int ano;
};

#define FILENAME "songs.csv" // Nome do arquivo CSV

// Função para ler músicas do arquivo CSV e carregá-las na memória
int ler_musicas(struct Music songs[]) {
    FILE *file = fopen(FILENAME, "r");
    if (file != NULL) {
        int numSongs = 0;
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            sscanf(line, "%d;%99[^;];%99[^;];%99[^;];%99[^;];%[^;];%d\n", &songs[numSongs].id, songs[numSongs].titulo, songs[numSongs].interprete, songs[numSongs].idioma, songs[numSongs].tipo, songs[numSongs].refrao, &songs[numSongs].ano);
            numSongs++;
            if (numSongs >= MAX_SONGS) {
                printf("Limite máximo de músicas alcançado!\n");
                break;
            }
        }
        fclose(file);
        return numSongs; // Retorna o número de músicas lidas
    } else {
        printf("Erro ao abrir o arquivo %s para leitura!\n", FILENAME);
        return 0; // Arquivo vazio ou falha ao abrir o arquivo
    }
}

// Função para escrever músicas no arquivo CSV
int escrever_musicas(struct Music songs[], int numSongs) {
    FILE *file = fopen(FILENAME, "w");
    if (file != NULL) {
        for (int i = 0; i < numSongs; i++) {
            fprintf(file, "%d;%s;%s;%s;%s;%s;%d\n", songs[i].id, songs[i].titulo, songs[i].interprete, songs[i].idioma, songs[i].tipo, songs[i].refrao, songs[i].ano);
        }
        fclose(file);
        return 1; // Sucesso ao escrever músicas
    } else {
        printf("Erro ao abrir o arquivo %s para escrita!\n", FILENAME);
        return 0; // Falha ao abrir o arquivo
    }
}

// Função para gerar um ID único para cada música
int gerar_id_unico(struct Music songs[], int numSongs) {
    int max_id = 0;
    for (int i = 0; i < numSongs; i++) {
        if (songs[i].id > max_id) {
            max_id = songs[i].id;
        }
    }
    // Incrementar o máximo ID em 1 para obter um ID único
    return max_id + 1;
}

// Função para cadastrar uma nova música
int cadastrar_musica(struct Music newSong, struct Music songs[], int *numSongs) {
    if (*numSongs < MAX_SONGS) {
        // Gerar um ID único para a nova música
        newSong.id = gerar_id_unico(songs, *numSongs);
        songs[(*numSongs)++] = newSong;
        if (escrever_musicas(songs, *numSongs))
            return 1; // Sucesso ao cadastrar e escrever músicas no arquivo
        else
            return 0; // Falha ao escrever músicas no arquivo
    } else {
        return 0; // Limite máximo de músicas alcançado
    }
}

// Função para remover uma música pelo ID
int remover_musica(int id, struct Music songs[], int *numSongs) {
    printf("IDs presentes na lista antes da remoção:\n");
    for (int i = 0; i < *numSongs; i++) {
        printf("%d ", songs[i].id);
    }
    printf("\n");
    
    
    int i, found = 0;

    for (i = 0; i < *numSongs; i++) {
        if (songs[i].id == id) {
            found = 1;
            break;
        }
    }
    if (found) {
        for (; i < *numSongs - 1; i++) {
            songs[i] = songs[i + 1];
        }
        (*numSongs)--;
        if (escrever_musicas(songs, *numSongs)) // Atualizar arquivo CSV após remover música
            return 1; // Sucesso ao remover e escrever músicas no arquivo
        else
            return 0; // Falha ao escrever músicas no arquivo
    } else {
        printf("Música com ID %d não encontrada.\n", id);
        return 0; // Música não encontrada
    }
}


// Função para listar músicas lançadas em um determinado ano
void listar_musicas_ano(int ano) {
    struct Music songs[MAX_SONGS];
    int numSongs = ler_musicas(songs);
    
    if (numSongs > 0) {
        printf("Músicas lançadas no ano %d:\n", ano);
        int found = 0;
        for (int i = 0; i < numSongs; i++) {
            if (songs[i].ano == ano) {
                printf("ID: %d, Título: %s, Intérprete: %s\n", songs[i].id, songs[i].titulo, songs[i].interprete);
                found = 1;
            }
        }
        if (!found) {
            printf("Nenhuma música encontrada para o ano %d.\n", ano);
        }
    } else {
        printf("Nenhuma música encontrada.\n");
    }
}

// Função para listar músicas lançadas em um determinado ano e de um determinado idioma
void listar_musicas_idioma_ano(char* idioma, int ano) {
    struct Music songs[MAX_SONGS];
    int numSongs = ler_musicas(songs);
    
    if (numSongs > 0) {
        printf("Músicas lançadas no ano %d e no idioma %s:\n", ano, idioma);
        int found = 0;
        for (int i = 0; i < numSongs; i++) {
            if (songs[i].ano == ano && strcmp(songs[i].idioma, idioma) == 0) {
                printf("ID: %d, Título: %s, Intérprete: %s\n", songs[i].id, songs[i].titulo, songs[i].interprete);
                found = 1;
            }
        }
        if (!found) {
            printf("Nenhuma música encontrada para o ano %d e idioma %s.\n", ano, idioma);
        }
    } else {
        printf("Nenhuma música encontrada.\n");
    }
}

// Função para listar músicas de um determinado tipo
void listar_musicas_tipo(char* tipo) {
    struct Music songs[MAX_SONGS];
    int numSongs = ler_musicas(songs);
    
    if (numSongs > 0) {
        printf("Músicas do tipo %s:\n", tipo);
        int found = 0;
        for (int i = 0; i < numSongs; i++) {
            if (strcmp(songs[i].tipo, tipo) == 0) {
                printf("ID: %d, Título: %s, Intérprete: %s\n", songs[i].id, songs[i].titulo, songs[i].interprete);
                found = 1;
            }
        }
        if (!found) {
            printf("Nenhuma música encontrada do tipo %s.\n", tipo);
        }
    } else {
        printf("Nenhuma música encontrada.\n");
    }
}


// Função para listar músicas por idioma e ano em uma string
char* listar_musicas_idioma_ano_string(char* idioma, int ano) {
    struct Music songs[MAX_SONGS];
    int numSongs = ler_musicas(songs);
    
    char* result = (char*) malloc(1000); // Inicializa a string de resultado com tamanho suficiente para a mensagem de erro
    result[0] = '\0'; // Garante que a string de resultado comece vazia

    if (numSongs > 0) {
        int found = 0;
        for (int i = 0; i < numSongs; i++) {
            if (songs[i].ano == ano && strcmp(songs[i].idioma, idioma) == 0) {
                char temp[1000]; // Buffer temporário para construção da string
                snprintf(temp, sizeof(temp), "ID: %d, Título: %s, Intérprete: %s\n", songs[i].id, songs[i].titulo, songs[i].interprete);
                strcat(result, temp); // Concatena a nova string ao resultado
                found = 1;
            }
        }
        if (!found) {
            strcpy(result, "Nenhuma música encontrada para o idioma e ano especificados.\n");
        }
    } else {
        strcpy(result, "Nenhuma música encontrada.\n");
    }

    return result;
}

// Função para listar músicas por tipo em uma string
char* listar_musicas_tipo_string(char* tipo) {
    struct Music songs[MAX_SONGS];
    int numSongs = ler_musicas(songs);
    
    char* result = (char*) malloc(1000); // Inicializa a string de resultado com tamanho suficiente para a mensagem de erro
    result[0] = '\0'; // Garante que a string de resultado comece vazia

    if (numSongs > 0) {
        int found = 0;
        for (int i = 0; i < numSongs; i++) {
            if (strcmp(songs[i].tipo, tipo) == 0) {
                char temp[1000]; // Buffer temporário para construção da string
                snprintf(temp, sizeof(temp), "ID: %d, Título: %s, Intérprete: %s\n", songs[i].id, songs[i].titulo, songs[i].interprete);
                strcat(result, temp); // Concatena a nova string ao resultado
                found = 1;
            }
        }
        if (!found) {
            strcpy(result, "Nenhuma música encontrada do tipo especificado.\n");
        }
    } else {
        strcpy(result, "Nenhuma música encontrada.\n");
    }

    return result;
}






// Função para listar informações de uma música pelo ID
void listar_informacoes_musica(int id) {
    struct Music songs[MAX_SONGS];
    int numSongs = ler_musicas(songs);
    
    if (numSongs > 0) {
        int found = 0;
        for (int i = 0; i < numSongs; i++) {
            if (songs[i].id == id) {
                printf("Informações da música com ID %d:\n", id);
                printf("Título: %s\n", songs[i].titulo);
                printf("Intérprete: %s\n", songs[i].interprete);
                printf("Idioma: %s\n", songs[i].idioma);
                printf("Tipo: %s\n", songs[i].tipo);
                printf("Refrão: %s\n", songs[i].refrao); // Mostrar o refrão
                printf("Ano: %d\n", songs[i].ano);
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("Nenhuma música encontrada com o ID %d.\n", id);
        }
    } else {
        printf("Nenhuma música encontrada.\n");
    }
}

// Função para listar informações de todas as músicas
void listar_informacoes_todas_musicas() {
    struct Music songs[MAX_SONGS];
    int numSongs = ler_musicas(songs);
    
    if (numSongs > 0) {
        printf("Informações de todas as músicas:\n");
        for (int i = 0; i < numSongs; i++) {
            printf("ID: %d\n", songs[i].id);
            printf("Título: %s\n", songs[i].titulo);
            printf("Intérprete: %s\n", songs[i].interprete);
            printf("Idioma: %s\n", songs[i].idioma);
            printf("Tipo: %s\n", songs[i].tipo);
            printf("Refrão: %s\n", songs[i].refrao); // Mostrar o refrão
            printf("Ano: %d\n", songs[i].ano);
            printf("\n");
        }
    } else {
        printf("Nenhuma música encontrada.\n");
    }
}


// Função para listar informações de uma música pelo ID em uma string
char* listar_informacoes_musica_string(int id) {
    struct Music songs[MAX_SONGS];
    int numSongs = ler_musicas(songs);
    
    char* result = (char*) malloc(1000); // Inicializa a strg de resultado com tamanho suficiente para a mensagem de erro
    result[0] = '\0'; // Garante que a string de resultado comece vazia

    if (numSongs > 0) {
        int found = 0;
        for (int i = 0; i < numSongs; i++) {
            if (songs[i].id == id) {
                snprintf(result, 1000, "Título: %s\nArtista: %s\nIdioma: %s\nTipo: %s\nRefrão: %s\nAno: %d\n",
                        songs[i].titulo, songs[i].interprete, songs[i].idioma, songs[i].tipo, songs[i].refrao, songs[i].ano);
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(result, "Nenhuma música encontrada com o ID especificado.\n");
        }
    } else {
        strcpy(result, "Nenhuma música encontrada.\n");
    }

    return result;
}

// Função para listar informações de todas as músicas em uma string
char* listar_informacoes_todas_musicas_string() {
    struct Music songs[MAX_SONGS];
    int numSongs = ler_musicas(songs);
    
    char* result = (char*) malloc(1000); // Inicializa a string de resultado com tamanho suficiente para a mensagem de erro
    result[0] = '\0'; // Garante que a string de resultado comece vazia

    if (numSongs > 0) {
        char temp[1000]; // Buffer temporário para construção da string
        for (int i = 0; i < numSongs; i++) {
            snprintf(temp, sizeof(temp), "ID: %d\nTítulo: %s\nArtista: %s\nIdioma: %s\nTipo: %s\nRefrão: %s\nAno: %d\n\n",
                     songs[i].id, songs[i].titulo, songs[i].interprete, songs[i].idioma, songs[i].tipo, songs[i].refrao, songs[i].ano);
            strcat(result, temp); // Concatena a nova string ao resultdo
        }
    } else {
        strcpy(result, "Nenhuma música encontrada.\n");
    }

    return result;
}

// Função para listar músicas lançadas em um determinado ano em uma string
char* listar_musicas_ano_string(int ano) {
    struct Music songs[MAX_SONGS];
    int numSongs = ler_musicas(songs);
    
    char* result = (char*) malloc(1000); // Inicializa a string de resultado com tamanho suficiente para a mensagem de erro
    result[0] = '\0'; // Garante que a string de resultado comece vazia

    if (numSongs > 0) {
        int found = 0;
        for (int i = 0; i < numSongs; i++) {
            if (songs[i].ano == ano) {
                char temp[1000]; // Buffer temporário para construção da string
                snprintf(temp, sizeof(temp), "ID: %d, Título: %s, Artista: %s\n", songs[i].id, songs[i].titulo, songs[i].interprete);
                strcat(result, temp); // Concatena a nova string ao resultado
                found = 1;
            }
        }
        if (!found) {
            strcpy(result, "Nenhuma música encontrada para o ano especificado.\n");
        }
    } else {
        strcpy(result, "Nenhuma música encontrada.\n");
    }

    return result;
}




void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0)
		;

	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}



#define MAX_PACKET_SIZE 1024 // Defina o tamanho máximo do pacote conforme necessário

struct Packet {
    int index;
    char data[MAX_PACKET_SIZE];
};

// Função para ler o arquivo MP3 e dividir em pacotes
struct Packet* divideFileInPackets(char *filename, int *numPackets) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Não foi possível abrir o arquivo %s\n", filename);
        return NULL;
    }

    // Obtém o tamanho do arquivo
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Calcula o número de pacotes
    *numPackets = fileSize / MAX_PACKET_SIZE;
    if (fileSize % MAX_PACKET_SIZE) (*numPackets)++;

    // Aloca memória para os pacotes
    struct Packet *packets = malloc((*numPackets) * sizeof(struct Packet));

    // Lê o arquivo e divide em pacotes
    for (int i = 0; i < *numPackets; i++) {
        packets[i].index = i;
        int bytesRead = fread(packets[i].data, 1, MAX_PACKET_SIZE, file);
        if (bytesRead < MAX_PACKET_SIZE) {
            // Se o último pacote for menor que o tamanho máximo do pacote, preenche com zeros
            memset(packets[i].data + bytesRead, 0, MAX_PACKET_SIZE - bytesRead);
        }
    }

    fclose(file);
    return packets;
}

// Função para enviar pacotes via UDP
void sendPacketsUDP(char *ip, struct Packet *packets, int numPackets) {
    for (int i = 0; i < numPackets; i++) {
        // Convertendo o índice do pacote para string
        char indexStr[10];
        sprintf(indexStr, "%d", packets[i].index);

        // Concatenando o índice do pacote e os dados do pacote
        char packetData[MAX_PACKET_SIZE + 10];
        strcpy(packetData, indexStr);

        strcat(packetData, packets[i].data);

        //printf(packetData[0]);

        // Enviando o pacote via UDP
        sendDataUDP(ip, packetData);
    }
}




int sendDataUDP(char *ip, char *data)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(ip, CLIENTPORT, &hints, &servinfo)) != 0)
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


int handleData(char *mensagem,int sockfd,char*ip)
{
	printf("server: received '%s'\n", mensagem);
	struct Music musicas[MAX_SONGS];
    int numMusicas = ler_musicas(musicas); // Lendo músicas do arquivo



    char lenbyte[3]; // Será o tamanho máximo de 2 dígitos + caractere nulo
    char op[2]; // Será o tamanho máximo de 1 dígito + caractere nulo
    char dados[100]; // Ajuste o tamanho conforme necessário
    
    // Usando sscanf para dividir a string
    sscanf(mensagem, "%[^|]|%[^|]|%[^\n]", lenbyte, op, dados);

    printf("len byte:%s\n", lenbyte);
    printf("op:%s\n", op);
    printf("dados:%s\n", dados);
    

    switch (atoi(op)) {
        case 6: {
            struct Music newSong;
            sscanf(dados, "%99[^|]|%99[^|]|%99[^|]|%99[^|]|%99[^|]|%d", newSong.titulo, newSong.interprete, newSong.idioma, newSong.tipo, newSong.refrao, &newSong.ano);
            if (cadastrar_musica(newSong, musicas, &numMusicas))
                sendData(sockfd, "Música cadastrada com sucesso!\n");
            else
                sendData(sockfd, "Erro ao cadastrar música!\n");
            break;
        }
        case 7: {
            int id_remover = atoi(dados);
            if (remover_musica(id_remover, musicas, &numMusicas))
                sendData(sockfd, "Música removida com sucesso!\n");
            else
                sendData(sockfd, "Erro ao remover música!\n");
            break;
        }
        case 1: {
            int ano = atoi(dados);
            printf("%s",listar_musicas_ano_string(ano));
            sendData(sockfd, listar_musicas_ano_string(ano));
            break;
        }
        case 2: {
            char idioma[100];
            int ano;
            sscanf(dados, "%[^|]|%d", idioma, &ano);
            printf("%s",listar_musicas_idioma_ano_string(idioma, ano));
            sendData(sockfd, listar_musicas_idioma_ano_string(idioma, ano));
            break;
        }

        case 3: {
            char tipo[100];
            sscanf(dados, "%[^\n]", tipo);
            printf("%s",listar_musicas_tipo_string(tipo));
            sendData(sockfd, listar_musicas_tipo_string(tipo));
            break;
        }

        case 4: {
            int id = atoi(dados);
            sendData(sockfd, listar_informacoes_musica_string(id));
            break;
        }
        case 5: {
            printf("%s",listar_informacoes_todas_musicas_string());
            sendData(sockfd, listar_informacoes_todas_musicas_string());
            break;
        }
        default:
            printf("Operação inválida!\n");
        case 8: {
            char *filename = "better-day-186374.mp3"; 
            int numPackets;
            struct Packet *packets = divideFileInPackets(filename, &numPackets);
            sendPacketsUDP(ip, packets, numPackets);
            free(packets);
            break;
        }
    }

    return 0;
}


int main(void)
{
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					   sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while (1)
	{ // main accept() loop
        // struct sockaddr_in client_addr;
        // socklen_t addr_size = sizeof(client_addr);
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1)
		{
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
				  get_in_addr((struct sockaddr *)&their_addr),
				  s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            char buf[MAXBUFLEN];
            ssize_t numBytes;

            // Loop para ler todas as solicitações até que o cliente feche a conexão
            while ((numBytes = recv(new_fd, buf, MAXBUFLEN - 1, 0)) > 0) {
                buf[numBytes] = '\0'; // Null-terminate a string
                // Verificação do tamanho da mensagem recebida
                int expected_size;
                if (sscanf(buf, "%d|", &expected_size) != 1) {
                    printf("Formato inválido da mensagem recebida.\n");
                    // Aqui você pode adicionar ações adicionais, como fechar a conexão ou enviar uma mensagem de erro
                } else {
                    if (numBytes != expected_size) {
                        printf("Tamanho da mensagem recebida incorreto.\n");
                        numBytes += recv(new_fd, buf, MAXBUFLEN - 1, 0);
                        // Aqui você pode adicionar ações adicionais, como fechar a conexão ou enviar uma mensagem de erro
                    } else {
                        handleData(buf, new_fd,s);

                    }
                }
            }

            if (numBytes == -1) {
                perror("recv");
            }

            close(new_fd);
            exit(0);
        }
        close(new_fd);  // pai não precisa disso
    }

    return 0;
}
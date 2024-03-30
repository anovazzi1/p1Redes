#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include "music.h" // Inclua o cabeçalho que define a estrutura de música*/
// music object structure

struct Music {
    int id;
    char titulo[100];
    char interprete[100];
    char idioma[100];
    char tipo[100];
    int ano;
};

#define MAX_SONGS 1000 // Defina o número máximo de músicas que o servidor pode armazenar
#define FILENAME "songs.csv" // Nome do arquivo CSV

// Função para ler as músicas do arquivo CSV e carregá-las na memória
int ler_musicas(struct Music songs[]) {
    FILE *file = fopen(FILENAME, "r");
    if (file != NULL) {
        int numSongs = 0;
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            sscanf(line, "%d,%99[^,],%99[^,],%99[^,],%99[^,],%d\n", &songs[numSongs].id, songs[numSongs].titulo, songs[numSongs].interprete, songs[numSongs].idioma, songs[numSongs].tipo, &songs[numSongs].ano);
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

// Função para escrever as músicas no arquivo CSV
int escrever_musicas(struct Music songs[], int numSongs) {
    FILE *file = fopen(FILENAME, "w");
    if (file != NULL) {
        for (int i = 0; i < numSongs; i++) {
            fprintf(file, "%d,%s,%s,%s,%s,%d\n", songs[i].id, songs[i].titulo, songs[i].interprete, songs[i].idioma, songs[i].tipo, songs[i].ano);
        }
        fclose(file);
        return 1; // Sucesso ao escrever as músicas
    } else {
        printf("Erro ao abrir o arquivo %s para escrita!\n", FILENAME);
        return 0; // Falha ao abrir o arquivo
    }
}

// Função para cadastrar uma nova música
int cadastrar_musica(struct Music newSong, struct Music songs[], int *numSongs) {
    if (*numSongs < MAX_SONGS) {
        songs[(*numSongs)++] = newSong;
        if (escrever_musicas(songs, *numSongs))
            return 1; // Sucesso ao cadastrar e escrever as músicas no arquivo
        else
            return 0; // Falha ao escrever as músicas no arquivo
    } else {
        return 0; // Limite máximo de músicas alcançado
    }
}

// Função para remover uma música pelo ID
int remover_musica(int id, struct Music songs[], int *numSongs) {
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
        if (escrever_musicas(songs, *numSongs))
            return 1; // Sucesso ao remover e escrever as músicas no arquivo
        else
            return 0; // Falha ao escrever as músicas no arquivo
    } else {
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
            printf("Ano: %d\n", songs[i].ano);
            printf("\n");
        }
    } else {
        printf("Nenhuma música encontrada.\n");
    }
}


int main() {
    // Cadastrar algumas músicas
    struct Music musicas[MAX_SONGS];
    int numMusicas = 0;

    struct Music musica1 = {1, "Bohemian Rhapsody", "Queen", "Inglês", "Rock", 1975};
    struct Music musica2 = {2, "Imagine", "John Lennon", "Inglês", "Rock", 1971};
    struct Music musica3 = {3, "La Vie En Rose", "Édith Piaf", "Francês", "Chanson", 1946};
    struct Music musica4 = {4, "Viva la Vida", "Coldplay", "Inglês", "Pop", 2008};

    if (cadastrar_musica(musica1, musicas, &numMusicas))
        printf("Música cadastrada com sucesso!\n");
    else
        printf("Erro ao cadastrar música!\n");

    if (cadastrar_musica(musica2, musicas, &numMusicas))
        printf("Música cadastrada com sucesso!\n");
    else
        printf("Erro ao cadastrar música!\n");

    if (cadastrar_musica(musica3, musicas, &numMusicas))
        printf("Música cadastrada com sucesso!\n");
    else
        printf("Erro ao cadastrar música!\n");

    if (cadastrar_musica(musica4, musicas, &numMusicas))
        printf("Música cadastrada com sucesso!\n");
    else
        printf("Erro ao cadastrar música!\n");

    // Listar todas as músicas
    listar_informacoes_todas_musicas();

    // Listar músicas lançadas em um determinado ano
    listar_musicas_ano(1971);

    // Listar músicas lançadas em um determinado ano e idioma
    listar_musicas_idioma_ano("Inglês", 1975);

    // Listar músicas de um determinado tipo
    listar_musicas_tipo("Rock");

    // Listar informações de uma música pelo ID
    listar_informacoes_musica(3);

    // Remover uma música pelo ID
    remover_musica(2, musicas, &numMusicas);

    // Listar novamente todas as músicas após a remoção
    listar_informacoes_todas_musicas();

    return 0;
}

main();
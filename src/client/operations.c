#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "music.h" // Inclua o cabeçalho que define a estrutura de música

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




// Any User operations
int listar_musicas_ano(int ano){

};
int listar_musicas_idioma_ano(char* idioma, int ano){

}; 
int listar_musicas_tipo(char* tipo){

};
int listar_informacoes_musica(int id){

};
int listar_informacoes_todas_musicas(){
    
};

// music object structure

struct Music {
    int id;
    char titulo[100];
    char interprete[100];
    char idioma[100];
    char tipo[100];
    int ano;
    char refrao[];
};
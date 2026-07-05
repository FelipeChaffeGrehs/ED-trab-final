#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

/* ---------- Estrutura de um caso (nó da lista encadeada) ---------- */
typedef struct Caso {
    char data_notific[11];   /* "AAAA-MM-DD\0" */
    char data_sintoma[11];
    int  ano;
    int  municipio;           /* código IBGE, ex: 432067 */
    float idade;               /* já decodificada (em anos) */
    char sexo;                 /* 'M' ou 'F' */
    int  evolucao;              /* 1=cura, 2=obito, 9=ignorado, 0=nao informado */
    int  criterio;
    struct Caso *prox;
} Caso;

/* ---------- Nó da árvore AVL (chave = município) ---------- */
typedef struct NoMunicipio {
    int municipio;
    Caso *lista_casos;         /* cabeça da lista, ordenada por data */
    int altura;
    struct NoMunicipio *esq;
    struct NoMunicipio *dir;
} NoMunicipio;

/* ---------- Funções da árvore AVL ---------- */
int max_int(int a, int b);
int altura_no(NoMunicipio *no);
int fator_balanceamento(NoMunicipio *no);
NoMunicipio* criar_no_municipio(int municipio);
NoMunicipio* rotacao_direita(NoMunicipio *y);
NoMunicipio* rotacao_esquerda(NoMunicipio *x);
NoMunicipio* inserir_municipio(NoMunicipio *raiz, int municipio, NoMunicipio **no_resultado);
NoMunicipio* buscar_municipio(NoMunicipio *raiz, int municipio);
void liberar_arvore(NoMunicipio *raiz);

/* ---------- Funções da lista encadeada de casos ---------- */
Caso* criar_caso(const char *data_notific, const char *data_sintoma, int ano,
                  int municipio, float idade, char sexo, int evolucao, int criterio);
void inserir_caso_ordenado(NoMunicipio *no, Caso *novo);
void buscar_por_periodo(Caso *lista, const char *data_ini, const char *data_fim);

/* ---------- Funções auxiliares de impressão (depuração) ---------- */
void imprimir_casos(Caso *lista);
void imprimir_arvore_em_ordem(NoMunicipio *raiz);

#endif
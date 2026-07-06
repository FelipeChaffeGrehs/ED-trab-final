#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estruturas.h"

/* ======================= FUNÇÕES AUXILIARES ======================= */

int max_int(int a, int b) {
    return (a > b) ? a : b;
}

int altura_no(NoMunicipio *no) {
    if (no == NULL) return 0;
    return no->altura;
}

int fator_balanceamento(NoMunicipio *no) {
    if (no == NULL) return 0;
    return altura_no(no->esq) - altura_no(no->dir);
}

NoMunicipio* criar_no_municipio(int municipio) {
    NoMunicipio *novo = (NoMunicipio*) malloc(sizeof(NoMunicipio));
    novo->municipio = municipio;
    novo->lista_casos = NULL;
    novo->altura = 1;
    novo->esq = NULL;
    novo->dir = NULL;
    return novo;
}

/* ======================= ROTAÇÕES DA AVL ======================= */

NoMunicipio* rotacao_direita(NoMunicipio *y) {
    NoMunicipio *x  = y->esq;
    NoMunicipio *t2 = x->dir;

    x->dir = y;
    y->esq = t2;

    y->altura = max_int(altura_no(y->esq), altura_no(y->dir)) + 1;
    x->altura = max_int(altura_no(x->esq), altura_no(x->dir)) + 1;

    return x; /* x vira a nova raiz desta subárvore */
}

NoMunicipio* rotacao_esquerda(NoMunicipio *x) {
    NoMunicipio *y  = x->dir;
    NoMunicipio *t2 = y->esq;

    y->esq = x;
    x->dir = t2;

    x->altura = max_int(altura_no(x->esq), altura_no(x->dir)) + 1;
    y->altura = max_int(altura_no(y->esq), altura_no(y->dir)) + 1;

    return y; /* y vira a nova raiz desta subárvore */
}

/* ======================= INSERÇÃO NA AVL ======================= */
/*
 * Insere o município na árvore, se ainda não existir, e rebalanceia.
 * Sempre deixa em *no_resultado o ponteiro para o nó daquele município
 * (recém-criado ou já existente), para o chamador poder inserir o caso
 * na lista encadeada dele.
 */
NoMunicipio* inserir_municipio(NoMunicipio *raiz, int municipio, NoMunicipio **no_resultado) {

    if (raiz == NULL) {
        NoMunicipio *novo = criar_no_municipio(municipio);
        *no_resultado = novo;
        return novo;
    }

    if (municipio < raiz->municipio) {
        raiz->esq = inserir_municipio(raiz->esq, municipio, no_resultado);
    } else if (municipio > raiz->municipio) {
        raiz->dir = inserir_municipio(raiz->dir, municipio, no_resultado);
    } else {
        *no_resultado = raiz;   /* município já existe, não duplica nó */
        return raiz;
    }

    /* Atualiza altura do nó atual */
    raiz->altura = 1 + max_int(altura_no(raiz->esq), altura_no(raiz->dir));

    /* Calcula fator de balanceamento e corrige se necessário */
    int fb = fator_balanceamento(raiz);

    /* Caso Esquerda-Esquerda */
    if (fb > 1 && municipio < raiz->esq->municipio)
        return rotacao_direita(raiz);

    /* Caso Direita-Direita */
    if (fb < -1 && municipio > raiz->dir->municipio)
        return rotacao_esquerda(raiz);

    /* Caso Esquerda-Direita */
    if (fb > 1 && municipio > raiz->esq->municipio) {
        raiz->esq = rotacao_esquerda(raiz->esq);
        return rotacao_direita(raiz);
    }

    /* Caso Direita-Esquerda */
    if (fb < -1 && municipio < raiz->dir->municipio) {
        raiz->dir = rotacao_direita(raiz->dir);
        return rotacao_esquerda(raiz);
    }

    return raiz; /* já estava balanceado */
}

/* ======================= BUSCA NA AVL ======================= */

NoMunicipio* buscar_municipio(NoMunicipio *raiz, int municipio) {
    if (raiz == NULL || raiz->municipio == municipio)
        return raiz;

    if (municipio < raiz->municipio)
        return buscar_municipio(raiz->esq, municipio);
    else
        return buscar_municipio(raiz->dir, municipio);
}

/* ======================= LISTA ENCADEADA DE CASOS ======================= */

Caso* criar_caso(const char *data_notific, const char *data_sintoma, int ano,
                  int municipio, float idade, char sexo, int evolucao, int criterio) {
    Caso *novo = (Caso*) malloc(sizeof(Caso));

    strncpy(novo->data_notific, data_notific, 10);
    novo->data_notific[10] = '\0';

    strncpy(novo->data_sintoma, data_sintoma, 10);
    novo->data_sintoma[10] = '\0';

    novo->ano = ano;
    novo->municipio = municipio;
    novo->idade = idade;
    novo->sexo = sexo;
    novo->evolucao = evolucao;
    novo->criterio = criterio;
    novo->prox = NULL;

    return novo;
}

/*
 * Insere o caso na lista do município, mantendo a ordem por data
 * (funciona porque a data está no formato AAAA-MM-DD: comparar como
 * string com strcmp já dá a ordem cronológica certa).
 */
void inserir_caso_ordenado(NoMunicipio *no, Caso *novo) {

    if (no->lista_casos == NULL ||
        strcmp(novo->data_notific, no->lista_casos->data_notific) <= 0) {
        novo->prox = no->lista_casos;
        no->lista_casos = novo;
        return;
    }

    Caso *atual = no->lista_casos;
    while (atual->prox != NULL &&
           strcmp(atual->prox->data_notific, novo->data_notific) < 0) {
        atual = atual->prox;
    }

    novo->prox = atual->prox;
    atual->prox = novo;
}

/* Busca (filtra) casos de uma lista dentro de um intervalo de datas */
void buscar_por_periodo(Caso *lista, const char *data_ini, const char *data_fim) {
    Caso *atual = lista;
    int encontrados = 0;

    while (atual != NULL) {
        if (strcmp(atual->data_notific, data_ini) >= 0 &&
            strcmp(atual->data_notific, data_fim) <= 0) {
            printf("  %s | idade: %.1f | sexo: %c | evolucao: %d\n",
                   atual->data_notific, atual->idade, atual->sexo, atual->evolucao);
            encontrados++;
        }
        atual = atual->prox;
    }

    if (encontrados == 0) {
        printf("  Nenhum caso encontrado no periodo.\n");
    }
}

/* ======================= IMPRESSÃO / DEPURAÇÃO ======================= */

void imprimir_casos(Caso *lista) {
    Caso *atual = lista;
    while (atual != NULL) {
        printf("  %s | idade: %.1f | sexo: %c | evolucao: %d\n",
               atual->data_notific, atual->idade, atual->sexo, atual->evolucao);
        atual = atual->prox;
    }
}

void imprimir_arvore_em_ordem(NoMunicipio *raiz) {
    if (raiz == NULL) return;
    imprimir_arvore_em_ordem(raiz->esq);
    printf("Municipio %d (altura na AVL: %d):\n", raiz->municipio, raiz->altura);
    imprimir_casos(raiz->lista_casos);
    imprimir_arvore_em_ordem(raiz->dir);
}

/* ======================= LIBERAÇÃO DE MEMÓRIA ======================= */

void liberar_arvore(NoMunicipio *raiz) {
    if (raiz == NULL) return;
    liberar_arvore(raiz->esq);
    liberar_arvore(raiz->dir);

    Caso *atual = raiz->lista_casos;
    while (atual != NULL) {
        Caso *tmp = atual;
        atual = atual->prox;
        free(tmp);
    }
    free(raiz);
}
#include <stdio.h>
#include <stdlib.h>
#include "estruturas.h"

/* Conta quantos casos de uma lista pertencem a um determinado ano */
static int contar_casos_por_ano(Caso *lista, int ano) {
    int contagem = 0;
    Caso *atual = lista;
    while (atual != NULL) {
        if (atual->ano == ano) contagem++;
        atual = atual->prox;
    }
    return contagem;
}

int main(void) {
    NoMunicipio *raiz = NULL;

    /* Carrega os três anos na mesma árvore. Cada município acumula os
     * casos dos 3 anos na sua lista, ordenados por data. */
    carregar_csv("lepto_rs_2023_final.csv", &raiz);
    carregar_csv("lepto_rs_2024_final.csv", &raiz);
    carregar_csv("lepto_rs_2025_final.csv", &raiz);

    if (raiz == NULL) {
        printf("\nNenhum dado foi carregado. Confira se os CSVs estao na mesma pasta do executavel.\n");
        return 1;
    }

    /* Exemplo: Porto Alegre (432067). Troque o codigo do municipio para
     * consultar outra cidade. */
    int municipio_teste = 432067;
    NoMunicipio *no = buscar_municipio(raiz, municipio_teste);

    if (no == NULL) {
        printf("\nMunicipio %d nao encontrado nos dados carregados.\n", municipio_teste);
    } else {
        int c2023 = contar_casos_por_ano(no->lista_casos, 2023);
        int c2024 = contar_casos_por_ano(no->lista_casos, 2024);
        int c2025 = contar_casos_por_ano(no->lista_casos, 2025);

        printf("\n=== Casos de leptospirose - municipio %d ===\n", municipio_teste);
        printf("2023: %d casos\n", c2023);
        printf("2024: %d casos  <-- ano da enchente\n", c2024);
        printf("2025: %d casos\n", c2025);

        printf("\n=== Casos notificados em abril/2024 (pico da enchente) ===\n");
        buscar_por_periodo(no->lista_casos, "2024-04-01", "2024-04-30");
    }

    liberar_arvore(raiz);
    return 0;
}
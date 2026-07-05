#include <stdio.h>
#include <stdlib.h>
#include "estruturas.h"

/*
 * Programa de TESTE: insere alguns casos manuais para validar que a
 * AVL e a lista encadeada estão funcionando antes de ligarmos a leitura
 * do CSV de verdade (isso vem na próxima etapa).
 */
int main(void) {
    NoMunicipio *raiz = NULL;
    NoMunicipio *no;

    struct {
        const char *data;
        int municipio;
        float idade;
        char sexo;
        int evolucao;
    } exemplos[] = {
        {"2024-04-18", 432067, 52.0f, 'M', 1},
        {"2024-04-15", 432067, 3.0f,  'F', 1},
        {"2023-01-02", 430640, 63.0f, 'M', 1},
        {"2024-05-01", 432067, 29.0f, 'F', 9},
        {"2024-04-10", 431440, 40.0f, 'M', 1},
        {"2025-02-20", 430640, 21.0f, 'F', 1},
    };

    int n = sizeof(exemplos) / sizeof(exemplos[0]);

    for (int i = 0; i < n; i++) {
        raiz = inserir_municipio(raiz, exemplos[i].municipio, &no);

        Caso *c = criar_caso(exemplos[i].data, exemplos[i].data, 2024,
                              exemplos[i].municipio, exemplos[i].idade,
                              exemplos[i].sexo, exemplos[i].evolucao, 1);

        inserir_caso_ordenado(no, c);
    }

    printf("=== Arvore em ordem (por municipio) ===\n");
    imprimir_arvore_em_ordem(raiz);

    printf("\n=== Busca pelo municipio 432067 (Porto Alegre) ===\n");
    NoMunicipio *encontrado = buscar_municipio(raiz, 432067);
    if (encontrado != NULL) {
        imprimir_casos(encontrado->lista_casos);
    } else {
        printf("Municipio nao encontrado.\n");
    }

    printf("\n=== Busca por periodo (abril/2024) em 432067 ===\n");
    if (encontrado != NULL) {
        buscar_por_periodo(encontrado->lista_casos, "2024-04-01", "2024-04-30");
    }

    liberar_arvore(raiz);
    return 0;
}
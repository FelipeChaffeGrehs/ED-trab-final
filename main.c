#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estruturas.h"

/* ===================== FUNÇÕES DE APOIO (RELATÓRIOS) ===================== */

static int contar_casos_por_ano(Caso *lista, int ano) {
    int contagem = 0;
    Caso *atual = lista;
    while (atual != NULL) {
        if (atual->ano == ano) contagem++;
        atual = atual->prox;
    }
    return contagem;
}

static int contar_total_casos(Caso *lista) {
    int total = 0;
    Caso *atual = lista;
    while (atual != NULL) {
        total++;
        atual = atual->prox;
    }
    return total;
}

/* Percorre a arvore em ordem e imprime um resumo (municipio + total de casos) */
static void listar_resumo_municipios(NoMunicipio *raiz) {
    if (raiz == NULL) return;
    listar_resumo_municipios(raiz->esq);
    printf("  Municipio %d: %d casos (2023=%d, 2024=%d, 2025=%d)\n",
           raiz->municipio,
           contar_total_casos(raiz->lista_casos),
           contar_casos_por_ano(raiz->lista_casos, 2023),
           contar_casos_por_ano(raiz->lista_casos, 2024),
           contar_casos_por_ano(raiz->lista_casos, 2025));
    listar_resumo_municipios(raiz->dir);
}

/* ===================== ENTRADA DO USUÁRIO ===================== */

/* Limpa o restante do buffer de entrada (necessario apos scanf de numero,
 * senao o Enter que sobra atrapalha a proxima leitura de texto) */
static void limpar_buffer_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static void menu_consultar_municipio(NoMunicipio *raiz) {
    int codigo;

    printf("\nDigite o codigo IBGE do municipio (ex: 431490 = Porto Alegre): ");
    if (scanf("%d", &codigo) != 1) {
        printf("Entrada invalida.\n");
        limpar_buffer_entrada();
        return;
    }
    limpar_buffer_entrada();

    NoMunicipio *no = buscar_municipio(raiz, codigo);
    if (no == NULL) {
        printf("Municipio %d nao encontrado nos dados carregados.\n", codigo);
        return;
    }

    printf("\n=== Casos de leptospirose - municipio %d ===\n", codigo);
    printf("2023: %d casos\n", contar_casos_por_ano(no->lista_casos, 2023));
    printf("2024: %d casos  <-- ano da enchente\n", contar_casos_por_ano(no->lista_casos, 2024));
    printf("2025: %d casos\n", contar_casos_por_ano(no->lista_casos, 2025));
    printf("Total: %d casos\n", contar_total_casos(no->lista_casos));

    char resposta = 'n';
    printf("\nDeseja filtrar por um periodo de datas? (s/n): ");
    if (scanf(" %c", &resposta) != 1) {
        limpar_buffer_entrada();
        return;
    }
    limpar_buffer_entrada();

    if (resposta == 's' || resposta == 'S') {
        char data_ini[32], data_fim[32];

        printf("Data inicial (formato AAAA-MM-DD, ex: 2024-04-01): ");
        fgets(data_ini, sizeof(data_ini), stdin);
        data_ini[strcspn(data_ini, "\r\n")] = '\0';

        printf("Data final   (formato AAAA-MM-DD, ex: 2024-04-30): ");
        fgets(data_fim, sizeof(data_fim), stdin);
        data_fim[strcspn(data_fim, "\r\n")] = '\0';

        printf("\n=== Casos entre %s e %s ===\n", data_ini, data_fim);
        buscar_por_periodo(no->lista_casos, data_ini, data_fim);
    }
}

/* ===================== PROGRAMA PRINCIPAL ===================== */

int main(void) {
    NoMunicipio *raiz = NULL;

    printf("Carregando dados...\n");
    carregar_csv("lepto_rs_2023_final.csv", &raiz);
    carregar_csv("lepto_rs_2024_final.csv", &raiz);
    carregar_csv("lepto_rs_2025_final.csv", &raiz);

    if (raiz == NULL) {
        printf("\nNenhum dado foi carregado. Confira se os CSVs estao na mesma pasta do executavel.\n");
        return 1;
    }

    int opcao;
    do {
        printf("\n===================================\n");
        printf(" Analise de Leptospirose - RS (2023-2025)\n");
        printf("===================================\n");
        printf("1. Consultar casos por municipio\n");
        printf("2. Listar todos os municipios carregados (resumo)\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida.\n");
            limpar_buffer_entrada();
            opcao = -1;
            continue;
        }
        limpar_buffer_entrada();

        switch (opcao) {
            case 1:
                menu_consultar_municipio(raiz);
                break;
            case 2:
                printf("\n=== Municipios carregados ===\n");
                listar_resumo_municipios(raiz);
                break;
            case 0:
                printf("Encerrando...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    liberar_arvore(raiz);
    return 0;
}
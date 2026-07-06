#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estruturas.h"

#define MAX_CAMPOS 20
#define TAM_CAMPO  64

/*
 * Divide uma linha do CSV em campos, separando por vírgula.
 * Importante: preserva campos vazios (ex: ",,") em vez de "pular" eles,
 * que é o erro mais comum ao usar strtok num CSV com dados faltando.
 */
static int dividir_linha_csv(char *linha, char campos[][TAM_CAMPO]) {
    int n = 0;
    char *inicio = linha;
    char *virgula;

    while ((virgula = strchr(inicio, ',')) != NULL && n < MAX_CAMPOS - 1) {
        int len = (int)(virgula - inicio);
        if (len >= TAM_CAMPO) len = TAM_CAMPO - 1;
        strncpy(campos[n], inicio, len);
        campos[n][len] = '\0';
        n++;
        inicio = virgula + 1;
    }

    /* último campo da linha (depois da última vírgula) */
    strncpy(campos[n], inicio, TAM_CAMPO - 1);
    campos[n][TAM_CAMPO - 1] = '\0';

    /* remove \n / \r do final da linha */
    int fim = (int)strlen(campos[n]) - 1;
    while (fim >= 0 && (campos[n][fim] == '\n' || campos[n][fim] == '\r')) {
        campos[n][fim] = '\0';
        fim--;
    }
    n++;

    return n; /* quantidade de campos encontrados */
}

/*
 * Ordem esperada das colunas no CSV (gerado pelo script Python):
 * 0 DT_NOTIFIC, 1 DT_SIN_PRI, 2 NU_ANO, 3 ID_MN_RESI,
 * 4 NU_IDADE_N, 5 CS_SEXO, 6 EVOLUCAO, 7 CRITERIO, 8 IDADE_ANOS
 */
int carregar_csv(const char *caminho, NoMunicipio **raiz) {
    FILE *f = fopen(caminho, "r");
    if (f == NULL) {
        printf("ERRO: nao consegui abrir o arquivo '%s'\n", caminho);
        return 0;
    }

    char linha[1024];
    char campos[MAX_CAMPOS][TAM_CAMPO];
    int total_lidos = 0;
    int total_ignorados = 0;

    /* pula a linha de cabeçalho */
    if (fgets(linha, sizeof(linha), f) == NULL) {
        fclose(f);
        return 0;
    }

    while (fgets(linha, sizeof(linha), f) != NULL) {
        int n = dividir_linha_csv(linha, campos);

        if (n < 8) {
            total_ignorados++;
            continue; /* linha incompleta/corrompida */
        }

        char *data_notific = campos[0];
        char *data_sintoma = campos[1];
        int   ano          = atoi(campos[2]);
        int   municipio    = atoi(campos[3]);
        char  sexo         = (strlen(campos[5]) > 0) ? campos[5][0] : '?';
        int   evolucao     = (strlen(campos[6]) > 0) ? (int) atof(campos[6]) : 0;
        int   criterio     = (strlen(campos[7]) > 0) ? (int) atof(campos[7]) : 0;
        float idade        = (n > 8 && strlen(campos[8]) > 0) ? (float) atof(campos[8]) : -1.0f;

        /* linha inválida: sem município ou sem data, não dá pra usar */
        if (municipio == 0 || strlen(data_notific) < 8) {
            total_ignorados++;
            continue;
        }

        NoMunicipio *no;
        *raiz = inserir_municipio(*raiz, municipio, &no);

        Caso *c = criar_caso(data_notific, data_sintoma, ano, municipio,
                              idade, sexo, evolucao, criterio);
        inserir_caso_ordenado(no, c);

        total_lidos++;
    }

    fclose(f);
    printf("Carregado '%s': %d registros validos, %d ignorados\n",
           caminho, total_lidos, total_ignorados);

    return total_lidos;
}
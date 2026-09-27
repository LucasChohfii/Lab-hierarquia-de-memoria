/* Uso: ./varredura_linha <N> - conta elementos pares percorrendo por linhas */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        return 1;
    }
    int N = atoi(argv[1]);
    if (N < 1) { fprintf(stderr, "N deve ser >= 1\n"); return 1; }

    double *A = malloc((size_t)N * N * sizeof(double));
    if (!A) { perror("malloc"); return 1; }

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            A[(size_t)i * N + j] = (double)(i + j);

    struct timespec ini, fim;
    clock_gettime(CLOCK_MONOTONIC, &ini);

    /* laço externo em i: percorre a memória na ordem em que ela está (row-major) */
    long pares = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if ((int)A[(size_t)i * N + j] % 2 == 0)
                pares++;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo = (fim.tv_sec - ini.tv_sec) + (fim.tv_nsec - ini.tv_nsec) / 1e9;

    printf("[Varredura Linha] N=%d | Pares=%ld | Tempo: %.6f s\n", N, pares, tempo);
    free(A);
    return 0;
}

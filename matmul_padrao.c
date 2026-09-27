/* Uso: ./matmul_padrao <N> - multiplicação canônica C = A x B (laços i, j, k) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        return 1;
    }
    int N = atoi(argv[1]);
    if (N < 1) { fprintf(stderr, "N deve ser >= 1\n"); return 1; }

    size_t elems = (size_t)N * N;
    double *A = malloc(elems * sizeof(double));
    double *B = malloc(elems * sizeof(double));
    double *C = malloc(elems * sizeof(double));
    if (!A || !B || !C) { perror("malloc"); return 1; }

    /* inicialização determinística exigida pelo enunciado */
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[(size_t)i * N + j] = (double)(i + j);
            B[(size_t)i * N + j] = (double)(i * j);
        }
    }
    memset(C, 0, elems * sizeof(double));

    struct timespec ini, fim;
    clock_gettime(CLOCK_MONOTONIC, &ini);

    /* B[k*N+j] salta uma linha inteira a cada iteração do laço interno */
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double soma = 0.0;
            for (int k = 0; k < N; k++)
                soma += A[(size_t)i * N + k] * B[(size_t)k * N + j];
            C[(size_t)i * N + j] = soma;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo = (fim.tv_sec - ini.tv_sec) + (fim.tv_nsec - ini.tv_nsec) / 1e9;
    double gflops = (2.0 * N * N * N) / tempo / 1e9;

    /* checksum para comparar a integridade numérica entre as versões */
    double soma = 0.0;
    for (size_t t = 0; t < elems; t++) soma += C[t];

    printf("[Matmul Padrao] N=%d | Tempo: %.4f s | GFLOPS: %.2f | Checksum: %.6e\n",
           N, tempo, gflops, soma);

    free(A); free(B); free(C);
    return 0;
}

/* Uso: ./matmul_bloco <N> <B> - multiplicação com blocagem (tiling) B x B */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <N> <B>\n", argv[0]);
        return 1;
    }
    int N = atoi(argv[1]);
    int Bsz = atoi(argv[2]);
    if (N < 1 || Bsz < 1) { fprintf(stderr, "N e B devem ser >= 1\n"); return 1; }

    size_t elems = (size_t)N * N;
    double *A = malloc(elems * sizeof(double));
    double *B = malloc(elems * sizeof(double));
    double *C = malloc(elems * sizeof(double));
    if (!A || !B || !C) { perror("malloc"); return 1; }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[(size_t)i * N + j] = (double)(i + j);
            B[(size_t)i * N + j] = (double)(i * j);
        }
    }
    memset(C, 0, elems * sizeof(double));

    struct timespec ini, fim;
    clock_gettime(CLOCK_MONOTONIC, &ini);

    /* sub-blocos B x B de A, B e C ficam residentes no cache;
     * ordem interna i, k, j mantém o acesso contíguo em B e C */
    for (int ii = 0; ii < N; ii += Bsz) {
        for (int jj = 0; jj < N; jj += Bsz) {
            for (int kk = 0; kk < N; kk += Bsz) {
                for (int i = ii; i < ii + Bsz && i < N; i++) {
                    for (int k = kk; k < kk + Bsz && k < N; k++) {
                        double r = A[(size_t)i * N + k];
                        for (int j = jj; j < jj + Bsz && j < N; j++)
                            C[(size_t)i * N + j] += r * B[(size_t)k * N + j];
                    }
                }
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo = (fim.tv_sec - ini.tv_sec) + (fim.tv_nsec - ini.tv_nsec) / 1e9;
    double gflops = (2.0 * N * N * N) / tempo / 1e9;

    double soma = 0.0;
    for (size_t t = 0; t < elems; t++) soma += C[t];

    printf("[Matmul Blocado] N=%d | B=%d | Tempo: %.4f s | GFLOPS: %.2f | Checksum: %.6e\n",
           N, Bsz, tempo, gflops, soma);

    free(A); free(B); free(C);
    return 0;
}

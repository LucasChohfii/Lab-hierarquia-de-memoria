/* Uso: ./matmul_pthreads_bloco <N> <B> <T> - Pthreads + blocagem:
 * cada thread processa uma faixa de linhas usando sub-blocos B x B */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

static int N, Bsz;
static double *A, *B, *C;

typedef struct {
    int linha_ini, linha_fim;
} args_t;

static void *worker(void *arg) {
    args_t *a = (args_t *)arg;
    for (int ii = a->linha_ini; ii < a->linha_fim; ii += Bsz) {
        for (int jj = 0; jj < N; jj += Bsz) {
            for (int kk = 0; kk < N; kk += Bsz) {
                for (int i = ii; i < ii + Bsz && i < a->linha_fim; i++) {
                    for (int k = kk; k < kk + Bsz && k < N; k++) {
                        double r = A[(size_t)i * N + k];
                        for (int j = jj; j < jj + Bsz && j < N; j++)
                            C[(size_t)i * N + j] += r * B[(size_t)k * N + j];
                    }
                }
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: %s <N> <B> <num_threads>\n", argv[0]);
        return 1;
    }
    N = atoi(argv[1]);
    Bsz = atoi(argv[2]);
    int T = atoi(argv[3]);
    if (N < 1 || Bsz < 1 || T < 1) {
        fprintf(stderr, "N, B e num_threads devem ser >= 1\n");
        return 1;
    }

    size_t elems = (size_t)N * N;
    A = malloc(elems * sizeof(double));
    B = malloc(elems * sizeof(double));
    C = malloc(elems * sizeof(double));
    if (!A || !B || !C) { perror("malloc"); return 1; }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[(size_t)i * N + j] = (double)(i + j);
            B[(size_t)i * N + j] = (double)(i * j);
        }
    }
    memset(C, 0, elems * sizeof(double));

    pthread_t *threads = malloc(T * sizeof(pthread_t));
    args_t *args = malloc(T * sizeof(args_t));
    if (!threads || !args) { perror("malloc"); return 1; }

    struct timespec ini, fim;
    clock_gettime(CLOCK_MONOTONIC, &ini);

    int linhas = N / T;
    for (int t = 0; t < T; t++) {
        args[t].linha_ini = t * linhas;
        args[t].linha_fim = (t == T - 1) ? N : (t + 1) * linhas;
        if (pthread_create(&threads[t], NULL, worker, &args[t]) != 0) {
            fprintf(stderr, "Erro ao criar a thread %d\n", t);
            return 1;
        }
    }
    for (int t = 0; t < T; t++)
        pthread_join(threads[t], NULL);

    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo = (fim.tv_sec - ini.tv_sec) + (fim.tv_nsec - ini.tv_nsec) / 1e9;
    double gflops = (2.0 * N * N * N) / tempo / 1e9;

    double soma = 0.0;
    for (size_t t = 0; t < elems; t++) soma += C[t];

    printf("[Matmul Pthreads+Bloco] N=%d | B=%d | Threads=%d | Tempo: %.4f s | GFLOPS: %.2f | Checksum: %.6e\n",
           N, Bsz, T, tempo, gflops, soma);

    free(A); free(B); free(C);
    free(threads); free(args);
    return 0;
}

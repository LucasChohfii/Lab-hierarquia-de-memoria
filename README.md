# Laboratório de Hierarquia de Memória, Cache e Pthreads

- Computação Paralela
- Lucas Chohfi Nigro - RA: 10437138

## Estrutura
- `varredura_linha.c` / `varredura_coluna.c` - Item 2: impacto da localidade espacial
- `matmul_padrao.c` / `matmul_bloco.c` - Item 3: multiplicação canônica e com blocagem (tiling)
- `matmul_pthreads.c` / `matmul_pthreads_bloco.c` - Item 4: versões paralelas com Pthreads
- `bench.sh` - coleta os tempos e gera os arquivos CSV
- `graficos.py` - calcula as tabelas e gera os gráficos
- `cachegrind/` - relatórios de profiling de cache

## Compilação
```bash
make          # gera as versões -O0 e -O3 de cada programa
make clean    # remove os executáveis
```

## Execução
```bash
./varredura_linha_O3 <N>
./varredura_coluna_O3 <N>
./matmul_padrao_O3 <N>
./matmul_bloco_O3 <N> <B>
./matmul_pthreads_O3 <N> <num_threads>
./matmul_pthreads_bloco_O3 <N> <B> <num_threads>
```

## Profiling de cache
```bash
valgrind --tool=cachegrind --cache-sim=yes ./matmul_padrao_O3 512
valgrind --tool=cachegrind --cache-sim=yes ./matmul_bloco_O3 512 64
```

## Ambiente de teste
- Intel Core i7-8650U (4 núcleos físicos, 8 lógicos), 16 GB de RAM
- Cache: L1d 32 KiB 8-way, L2 256 KiB 4-way, L3 8 MiB 16-way, linha de 64 bytes
- Ubuntu 24.04.4 LTS, kernel 7.0.0-31, gcc 13.3.0, Valgrind 3.22.0

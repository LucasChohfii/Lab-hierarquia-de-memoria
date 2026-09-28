#!/bin/bash
# Coleta todas as medições do laboratório e grava em arquivos CSV
# (uma execução por configuração, como reportado no relatório).
# Uso: bash bench.sh

set -e
tempo_de() { grep -oP 'Tempo: \K[0-9.]+'; }

# --- Tabela 1: varredura por linha vs por coluna ---
echo "N,versao,tempo" > res_varredura.csv
for N in 512 1024 2048 4096 8192; do
    t=$(./varredura_linha_O3 $N | tempo_de);  echo "$N,linha,$t"  >> res_varredura.csv
    t=$(./varredura_coluna_O3 $N | tempo_de); echo "$N,coluna,$t" >> res_varredura.csv
    echo "varredura N=$N concluida"
done

# --- Tabela 2: matmul padrao vs blocado, -O0 vs -O3 ---
echo "N,versao,opt,tempo" > res_matmul.csv
for N in 512 1024 1536; do
    for opt in O0 O3; do
        t=$(./matmul_padrao_$opt $N | tempo_de);   echo "$N,padrao,$opt,$t" >> res_matmul.csv
        t=$(./matmul_bloco_$opt $N 64 | tempo_de); echo "$N,bloco,$opt,$t"  >> res_matmul.csv
    done
    echo "matmul N=$N concluido"
done

# --- Calibração do tamanho do bloco B (N = 1024, -O3) ---
echo "B,tempo" > res_bloco.csv
for B in 8 16 32 64 128 256; do
    t=$(./matmul_bloco_O3 1024 $B | tempo_de); echo "$B,$t" >> res_bloco.csv
done
echo "calibracao de B concluida"

# --- Tabela 3: escalabilidade com Pthreads (N = 1024) ---
echo "versao,threads,tempo" > res_pthreads.csv
for T in 1 2 4 8 16; do
    t=$(./matmul_pthreads_O3 1024 $T | tempo_de);          echo "pthreads,$T,$t"       >> res_pthreads.csv
    t=$(./matmul_pthreads_bloco_O3 1024 64 $T | tempo_de); echo "pthreads_bloco,$T,$t" >> res_pthreads.csv
    echo "pthreads T=$T concluido"
done

echo "FIM: res_varredura.csv, res_matmul.csv, res_bloco.csv, res_pthreads.csv"

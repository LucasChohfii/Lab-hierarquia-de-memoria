#!/bin/bash
# 1) medições limpas  2) profiling de cache (sem concorrência entre eles)
bash bench.sh > bench_log.txt 2>&1
echo "BENCH LIMPO OK"
mkdir -p cachegrind
run_cg() { # $1=nome  $2..=argumentos
  local nome=$1; shift
  valgrind --tool=cachegrind --cache-sim=yes \
      --cachegrind-out-file=cachegrind/cg_$nome.out ./$nome "$@" \
      > cachegrind/$nome.txt 2>&1
  echo "cachegrind $nome $* OK"
}
run_cg matmul_padrao_O3 1024
run_cg matmul_bloco_O3 1024 64
run_cg matmul_padrao_O0 512
run_cg matmul_bloco_O0 512 64
run_cg varredura_linha_O3 1024
run_cg varredura_coluna_O3 1024
echo "TUDO CONCLUIDO"

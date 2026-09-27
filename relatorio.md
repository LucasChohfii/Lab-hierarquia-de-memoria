# Laboratório de Hierarquia de Memória, Cache e Pthreads — Relatório
- **Curso:** Ciência da Computação
- **Disciplina:** Computação Paralela
- **Nome:** Lucas Chohfi Nigro
- **RA:** 10437138
- **Data:** 27 de Setembro de 2026
- **Repositório:** [link]

---

## 1. Introdução e Fundamentos Teóricos

[Escrever: hierarquia de memória (registradores, L1/L2/L3 em SRAM, DRAM); localidade
espacial e temporal; linha de cache de 64 bytes; ordem row-major do C; mecânica da
blocagem (tiling); paralelismo em memória compartilhada com Pthreads.]

---

## 2. Metodologia e Caracterização do Hardware

- **Processador:** Intel Core i7-8650U @ 1,90 GHz (turbo 4,20 GHz)
- **Núcleos:** 4 físicos / 8 lógicos (Hyper-Threading)
- **Cache L1d:** 32 KiB por núcleo, 8-way associativa, linha de 64 bytes, 64 conjuntos
- **Cache L1i:** 32 KiB por núcleo, 8-way associativa, linha de 64 bytes
- **Cache L2:** 256 KiB por núcleo, 4-way associativa, linha de 64 bytes
- **Cache L3:** 8 MiB compartilhada, 16-way associativa, linha de 64 bytes
- **RAM:** 16 GB DDR4
- **Sistema operacional:** Ubuntu 24.04.4 LTS, kernel 7.0.0-31-generic
- **Compilador:** gcc 13.3.0, flags `-O0 -Wall -pthread` e `-O3 -Wall -pthread`
- **Profiling:** Valgrind 3.22.0, `valgrind --tool=cachegrind --cache-sim=yes ./programa <args>`

**Metodologia de medição:**
[Descrever: clock_gettime(CLOCK_MONOTONIC); 3 execuções por configuração e média;
matrizes alocadas como bloco contíguo (A[i*N+j]) e inicializadas com A=(i+j), B=(i*j);
checksum da matriz C usado para confirmar que todas as versões produzem o mesmo resultado;
notebook na tomada e sem outros programas abertos; a simulação do cachegrind é cerca de
50 vezes mais lenta que a execução nativa, por isso o profiling foi feito em N = 512 e
N = 1024, enquanto os tempos de N = 1536 foram medidos apenas nativamente.]

---

## 3. Evidências Experimentais

### 3.1 Compilação

[Print do `make`: 12 executáveis (-O0 e -O3), sem warnings.]

### 3.2 Item 2 — Varredura por linha e por coluna

[Print das execuções mostrando a mesma contagem de pares e a diferença de tempo.]

### 3.3 Item 3 — Multiplicação padrão e blocada, com o cachegrind

[Prints: execuções sob -O0 e -O3, e as saídas do cachegrind com as taxas de miss.]

### 3.4 Item 4 — Versões paralelas

[Prints com 1, 2, 4, 8 e 16 threads, mostrando o mesmo checksum.]

---

## 4. Tabelas de Desempenho e Gráficos

### 4.1 Tabela 1 — Localidade espacial (Item 2)

[Tabela preenchida pelo script: N, tempo linha, tempo coluna, slowdown.]

### 4.2 Tabela 2 — Matmul padrão × blocado, -O0 × -O3 (Item 3)

[Tabela: N, versão, otimização, tempo, D1 miss rate, LLd misses.]

### 4.3 Calibração do tamanho do bloco B

[Tabela: tempo por valor de B, com a justificativa do B escolhido.]

### 4.4 Tabela 3 — Escalabilidade com Pthreads (Item 4)

[Tabela: threads, tempo, speedup, eficiência, para as duas versões paralelas.]

### 4.5 Gráficos

[Gráfico de speedup × threads e gráfico de barras dos cache misses.]

---

## 5. Respostas às Questões de Reflexão

### Q1. Localidade espacial na varredura (Item 2)
[Por que row-major é mais rápido; quantos elementos cabem em uma linha de cache;
taxa de hits resultante; usar o slowdown medido.]

### Q2. Diagnóstico de faltas no algoritmo canônico (Item 3)
[Qual matriz causa a maioria dos misses no laço interno e por quê.]

### Q3. Princípio matemático da blocagem (Item 3)
[Como o tiling converte acessos à DRAM em acessos a L1/L2; critério para escolher B
a partir da capacidade da cache.]

### Q4. Impacto das otimizações do compilador (-O0 × -O3)
[Transformações aplicadas pelo -O3; se ele compensa um algoritmo com má localidade.]

### Q5. Análise forense das métricas do cachegrind (Item 3)
[Comparar D1 miss rate e LLd miss rate entre as versões; por que uma pequena redução
percentual em LL gera impacto desproporcional no tempo.]

### Q6. Escalabilidade e limites de barramento (Item 4)
[Speedup de 1 a 16 threads; gargalos: banda de memória e disputa pela L3 compartilhada;
4 núcleos físicos contra 8 lógicos.]

### Q7. Prevenção de falsa partilha (Item 4)
[Como a divisão por faixas de linhas evitou falsa partilha; o que aconteceria com
divisão elemento a elemento ou por coluna.]

### Q8. Sinergia entre blocagem e concorrência (Item 4)
[Se os ganhos são aditivos ou sinérgicos; como a redução de tráfego na DRAM melhora
a escalabilidade.]

---

## 6. Dificuldades Técnicas e Soluções

[Relatar: calibração de B; uso de bloco contíguo em vez de double**; o cachegrind do
Valgrind 3.22 exigir --cache-sim=yes; o perf bloqueado por perf_event_paranoid = 4;
o custo de tempo da simulação; interferência entre medições simultâneas.]

---

## 7. Declaração e Análise do Uso de IA

**Ferramentas utilizadas:**

**Onde foi utilizada:**

**Principais solicitações:**

**Análise crítica:**

---

## 8. Conclusão e Referências

[Conclusão.]

**Referências:**
- HENNESSY, J. L.; PATTERSON, D. A. Arquitetura de Computadores: uma abordagem quantitativa.
- BRYANT, R. E.; O'HALLARON, D. R. Computer Systems: A Programmer's Perspective.
- Documentação do Valgrind/Cachegrind. Disponível em: https://valgrind.org/docs/manual/cg-manual.html
- [Material didático da disciplina]

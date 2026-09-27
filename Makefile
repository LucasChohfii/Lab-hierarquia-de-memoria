CC = gcc
CFLAGS = -Wall -pthread

SRCS = varredura_linha varredura_coluna matmul_padrao matmul_bloco \
       matmul_pthreads matmul_pthreads_bloco

O0 = $(addsuffix _O0,$(SRCS))
O3 = $(addsuffix _O3,$(SRCS))

all: $(O0) $(O3)

%_O0: %.c
	$(CC) -O0 $(CFLAGS) $< -o $@

%_O3: %.c
	$(CC) -O3 $(CFLAGS) $< -o $@

clean:
	rm -f $(O0) $(O3) cachegrind.out.*

.PHONY: all clean

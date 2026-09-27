# Lê os CSVs e os relatórios do cachegrind, imprime as tabelas do relatório
# e gera os gráficos speedup.png, slowdown.png e cache_misses.png
import csv, glob, os, re
from collections import defaultdict
import matplotlib.pyplot as plt


def medias(arquivo, chaves, valor="tempo"):
    d = defaultdict(list)
    for linha in csv.DictReader(open(arquivo)):
        d[tuple(linha[k] for k in chaves)].append(float(linha[valor]))
    return {k: sum(v) / len(v) for k, v in d.items()}


# ---------- Tabela 1: varredura por linha x por coluna ----------
v = medias("res_varredura.csv", ["N", "versao"])
Ns = ["512", "1024", "2048", "4096", "8192"]
print("\nTabela 1 - Localidade espacial")
print("N      | T_linha (s) | T_coluna (s) | Slowdown")
slowdowns = []
for N in Ns:
    tl, tc = v[(N, "linha")], v[(N, "coluna")]
    slowdowns.append(tc / tl)
    print(f"{N:>6} | {tl:11.6f} | {tc:12.6f} | {tc/tl:7.2f}x")

plt.figure(figsize=(7, 4.5))
plt.plot([int(n) for n in Ns], slowdowns, marker="o", color="tab:red")
plt.xscale("log", base=2)
plt.xticks([int(n) for n in Ns], Ns)
plt.xlabel("Ordem da matriz (N)")
plt.ylabel("Slowdown (T_coluna / T_linha)")
plt.title("Impacto da localidade espacial")
plt.grid(True, alpha=0.4)
plt.tight_layout()
plt.savefig("slowdown.png", dpi=150)
plt.close()

# ---------- Tabela 2: matmul padrao x blocado, -O0 x -O3 ----------
m = medias("res_matmul.csv", ["N", "versao", "opt"])
print("\nTabela 2 - Multiplicacao de matrizes (tempo em s)")
print("N      | versao | -O0     | -O3     | ganho O3")
for N in ["512", "1024", "1536"]:
    for ver in ["padrao", "bloco"]:
        t0, t3 = m[(N, ver, "O0")], m[(N, ver, "O3")]
        print(f"{N:>6} | {ver:6} | {t0:7.4f} | {t3:7.4f} | {t0/t3:6.2f}x")
for N in ["512", "1024", "1536"]:
    print(f"  N={N}: blocagem acelera {m[(N,'padrao','O3')]/m[(N,'bloco','O3')]:.2f}x sob -O3")

# ---------- Calibracao do tamanho do bloco ----------
print("\nCalibracao do bloco B (N=1024, -O3)")
for linha in csv.DictReader(open("res_bloco.csv")):
    print(f"  B={linha['B']:>4}: {float(linha['tempo']):.4f} s")

# ---------- Tabela 3: escalabilidade com Pthreads ----------
p = medias("res_pthreads.csv", ["versao", "threads"])
Ts = ["1", "2", "4", "8", "16"]
print("\nTabela 3 - Escalabilidade (N=1024)")
print("versao          | T  | tempo (s) | Speedup | Eficiencia")
for ver in ["pthreads", "pthreads_bloco"]:
    t1 = p[(ver, "1")]
    sps = []
    for T in Ts:
        tp = p[(ver, T)]
        sp = t1 / tp
        sps.append(sp)
        print(f"{ver:15} | {T:>2} | {tp:9.4f} | {sp:7.2f} | {sp/int(T):10.2f}")
    plt.plot([int(t) for t in Ts], sps, marker="o", label=ver)

plt.plot([int(t) for t in Ts], [int(t) for t in Ts], "--", color="gray", label="ideal")
plt.xlabel("Número de threads")
plt.ylabel("Speedup (T1 / Tp)")
plt.xticks([int(t) for t in Ts], Ts)
plt.title("Escalabilidade da multiplicação de matrizes (N=1024)")
plt.legend()
plt.grid(True, alpha=0.4)
plt.tight_layout()
plt.savefig("speedup.png", dpi=150)
plt.close()

# ---------- Cache misses medidos pelo cachegrind ----------
def metricas(caminho):
    texto = open(caminho).read()
    def pega(rotulo):
        m = re.search(rotulo + r":\s+([\d,]+)", texto)
        return int(m.group(1).replace(",", "")) if m else None
    def taxa(rotulo):
        m = re.search(rotulo + r" rate:\s+([\d.]+)%", texto)
        return float(m.group(1)) if m else None
    return {"D1": pega("D1  misses"), "LLd": pega("LLd misses"),
            "D1r": taxa("D1  miss"), "LLdr": taxa("LLd miss")}

arquivos = sorted(glob.glob("cachegrind/*.txt"))
if arquivos:
    print("\nCachegrind")
    print("programa                  | D1 misses   | D1 rate | LLd misses | LLd rate")
    nomes, d1s, llds = [], [], []
    for a in arquivos:
        nome = os.path.basename(a)[:-4]
        r = metricas(a)
        if r["D1"] is None:
            continue
        print(f"{nome:25} | {r['D1']:11,} | {r['D1r']:6.1f}% | {r['LLd']:10,} | {r['LLdr']:6.1f}%")
        if nome.startswith("matmul"):
            nomes.append(nome.replace("matmul_", "").replace("_", "\n"))
            d1s.append(r["D1"])
            llds.append(r["LLd"])
    if nomes:
        x = range(len(nomes))
        fig, eixos = plt.subplots(1, 2, figsize=(11, 4.5))
        eixos[0].bar(x, d1s, color="tab:blue")
        eixos[0].set_title("Faltas em L1d (D1 misses)")
        eixos[1].bar(x, llds, color="tab:orange")
        eixos[1].set_title("Faltas no último nível (LLd misses)")
        for eixo in eixos:
            eixo.set_xticks(list(x))
            eixo.set_xticklabels(nomes, fontsize=8)
            eixo.set_yscale("log")
            eixo.grid(True, axis="y", alpha=0.4)
        plt.tight_layout()
        plt.savefig("cache_misses.png", dpi=150)
        plt.close()
        print("\nGraficos salvos: slowdown.png, speedup.png, cache_misses.png")

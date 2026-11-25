# Specifikace experimentu 002-sigmoid

- **Projekt:** 002-sigmoid
- **Revize:** 1
- **Stav:** rozpracovaný návrh; implementace nebyla schválena

## 1. Cíl

Navázat na experiment lineární regrese a ověřit, co do modelu přináší
sigmoidální aktivační funkce. Model tvaru $\hat y=ax+b$ s přímým
vstupem $x$ umí vyjádřit jen přímku; body s nelineárním průběhem
proto obecně přesně neproloží. Cílem je porozumět tomu, jak vložení
nelinearity mění množinu vztahů, které může model vyjádřit. Konkrétní
architekturu a ověřovací úlohu ještě společně určíme.

## 2. Teorie konceptu

Složení dvou afinních funkcí zůstává afinní:

$$
f(x)=ax+b,\qquad g(z)=cz+d,\qquad
g(f(x))=(ca)x+(cb+d).
$$

Samotné vrstvení afinních transformací tedy nezvětší množinu průběhů
nad afinní funkce. Sigmoidální aktivace zavádí nelinearitu:

$$
\sigma(z)=\frac{1}{1+\mathrm{e}^{-z}},\qquad
\hat y=\sigma(ax+b).
$$

Jediná taková jednotka má omezený, monotónní průběh ve tvaru S.
Pro modelování složitějších průběhů je třeba navrhnout vhodné
kombinace nelineárních jednotek. Lineární regrese může zachytit
nelineární vztah také tehdy, když jí předem připravíme nelineární
příznaky vstupu; zde zkoumáme nelinearitu v samotném modelu.

## 3. Otevřené návrhové otázky

Před implementací společně určíme architekturu modelu, trénovací
a validační data, ztrátovou funkci, optimalizaci a způsob porovnání
s afinním modelem. Teprve podle těchto rozhodnutí doplníme diagram,
rozhraní, metodiku a měřitelné podmínky experimentu.

## Slovník pojmů

- **Afinní funkce:** vztah $ax+b$, který tvoří výchozí model
  experimentu; skládáním takových funkcí vznikne opět afinní funkce.
- **Aktivační funkce:** funkce použitá na výstup transformace modelu;
  zde má zavést nelineární závislost na vstupu.
- **Sigmoida:** aktivační funkce $\sigma(z)$ s hodnotami mezi $0$ a $1$.
- **Příznak:** hodnota odvozená ze vstupu; nelineární příznak může
  rozšířit možnosti jinak afinního modelu.

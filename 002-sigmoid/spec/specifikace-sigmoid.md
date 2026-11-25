# Specifikace experimentu 002-sigmoid

- **Projekt:** 002-sigmoid
- **Revize:** 2
- **Stav:** rozpracovaný návrh; implementace nebyla schválena

## 1. Cíl

Navázat na lineární regresi a ověřit průběh jedné sigmoidální
jednotky. Afinní model $\hat y=ax+b$ s přímým vstupem $x$
vyjadřuje přímku. Sigmoidální aktivace mění jeho průběh na
nelineární křivku tvaru S. Konkrétní úlohu a podmínky ověření
ještě společně určíme.

## 2. Teorie konceptu

Složení dvou afinních funkcí zůstává afinní:

$$
f(x)=ax+b,\qquad g(z)=cz+d,\qquad
g(f(x))=(ca)x+(cb+d).
$$

Sigmoidální aktivace zavádí nelinearitu:

$$
\sigma(z)=\frac{1}{1+\mathrm{e}^{-z}},\qquad
\hat y=\sigma(ax+b).
$$

Jediná sigmoidální jednotka vytváří monotónní křivku tvaru S
s výstupem mezi $0$ a $1$. Její průběh tedy není přímkou,
ale nedokáže vyjádřit libovolný nelineární tvar.

## 3. Otevřené návrhové otázky

Před implementací společně určíme data, ztrátovou funkci,
optimalizaci a způsob porovnání s afinním modelem. Podle těchto
rozhodnutí doplníme diagram, rozhraní a metodiku experimentu.

## Slovník pojmů

- **Afinní funkce:** vztah $ax+b$, který tvoří výchozí model
  experimentu.
- **Aktivační funkce:** funkce použitá na výstup afinní transformace;
  zde mění přímku na nelineární průběh.
- **Sigmoida:** funkce $\sigma(z)$ s monotónní křivkou tvaru S
  a hodnotami mezi $0$ a $1$.

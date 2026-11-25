# Specifikace experimentu 002-perceptron

- **Projekt:** 002-perceptron
- **Revize:** 3
- **Stav:** rozpracovaný návrh; implementace nebyla schválena
- **Zvolená aktivační funkce:** sigmoida

## 1. Cíl

Navázat na lineární regresi a ověřit model jediného neuronu,
v tomto experimentu nazývaný perceptron. Afinní část
$z=ax+b$ s přímým vstupem $x$ vyjadřuje přímku.
Zvolená sigmoidální aktivace mění jeho výstup na nelineární
křivku tvaru S. Konkrétní úlohu a podmínky ověření ještě
společně určíme.

## 2. Teorie konceptu

Složení dvou afinních funkcí zůstává afinní:

$$
f(x)=ax+b,\qquad g(z)=cz+d,\qquad
g(f(x))=(ca)x+(cb+d).
$$

Zvolená aktivační funkce zavádí nelinearitu:

$$
\sigma(z)=\frac{1}{1+\mathrm{e}^{-z}},\qquad
\hat y=\sigma(ax+b).
$$

Jediný neuron se sigmoidální aktivací vytváří monotónní
křivku tvaru S s výstupem mezi $0$ a $1$. Jeho průběh tedy
není přímkou, ale nedokáže vyjádřit libovolný nelineární tvar.

## 3. Otevřené návrhové otázky

Před implementací společně určíme data, ztrátovou funkci,
optimalizaci a způsob porovnání s afinním modelem. Podle těchto
rozhodnutí doplníme diagram, rozhraní a metodiku experimentu.

## Slovník pojmů

- **Afinní funkce:** vztah $ax+b$, který tvoří vstupní část modelu.
- **Perceptron:** v tomto experimentu model jediného neuronu se
  sigmoidální aktivací; původní perceptron používal prahovou aktivaci.
- **Aktivační funkce:** funkce použitá na výstup afinní části neuronu;
  zde mění přímku na nelineární průběh.
- **Sigmoida:** zvolená aktivační funkce $\sigma(z)$ s monotónní
  křivkou tvaru S a hodnotami mezi $0$ a $1$.

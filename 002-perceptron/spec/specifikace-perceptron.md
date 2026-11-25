# Specifikace experimentu 002-perceptron

- **Projekt:** 002-perceptron
- **Revize:** 10
- **Stav:** rozpracovaný návrh; implementace nebyla schválena
- **Zvolená aktivační funkce:** sigmoida

## 1. Cíl

Navázat na lineární regresi a ověřit model jediného neuronu.
Teoretický model připouští $n$ vstupů, ale tento experiment
používá dvě váhy pro dva vstupy určené při překladu.
Sigmoidální aktivace mění afinní výpočet na nelineární výstup.
První úlohou je naučit logickou funkci AND se dvěma binárními vstupy. Pro učení
volíme MSE; postup optimalizace a podmínky ověření ještě určíme.

## 2. Teorie konceptu

Složení dvou afinních funkcí zůstává afinní:

$$
f(x)=ax+b,\qquad g(z)=cz+d,\qquad
g(f(x))=(ca)x+(cb+d).
$$

Pro $n\geq 1$ vstupů $\mathbf{x}\in\mathbb{R}^n$, váhy
$\mathbf{w}\in\mathbb{R}^n$ a bias $b\in\mathbb{R}$ počítá
jediný neuron

$$
z=\mathbf{w}^{\mathsf T}\mathbf{x}+b
  =\sum_{i=1}^{n}w_i x_i+b,\qquad
\hat y=\sigma(z),\qquad
\sigma(z)=\frac{1}{1+\mathrm{e}^{-z}}.
$$

Váhy $w_i$ a bias $b$ jsou učitelné **parametry**. Počet vstupů
$n$ určuje tvar modelu; volba aktivace a rychlosti učení patří
mezi **hyperparametry** modelu či trénování. Rychlost učení
neovlivňuje výpočet již natrénovaného neuronu.

Pro jeden vstup, $w_1=1$ a $b=0$, porovnává následující
teoretický graf afinní výstup $z=x$ a aktivovaný výstup
$\hat y=\sigma(x)$. Panely mají odlišné měřítko svislé osy.

![Teoretické porovnání afinní funkce a sigmoidální aktivace](graf-affinni-sigmoida.svg)

Sigmoidální výstup má při změně $z$ monotónní průběh
tvaru S a leží mezi $0$ a $1$. Pro více vstupů se mění
$z$ podle váženého součtu; jeden neuron stále neumí vyjádřit
libovolný nelineární vztah.

## 3. Data a ztrátová funkce

První úloha vychází z pravdivostní tabulky AND, tedy $n=2$
vstupy a čtyři možné binární kombinace. Symbol $n$ značí
počet vstupů neuronu, $N$ počet vzorků v dávce:

| $x_1$ | $x_2$ | Cíl $y=x_1\land x_2$ |
|---:|---:|---:|
| 0 | 0 | 0 |
| 0 | 1 | 0 |
| 1 | 0 | 0 |
| 1 | 1 | 1 |

Pro trénovací dávku o $N>0$ vzorcích volíme střední kvadratickou chybu

$$
\operatorname{MSE}(\mathbf{w},b)
=\frac{1}{N}\sum_{k=1}^{N}(\hat y_k-y_k)^2,
\qquad
\hat y_k=\sigma\!\left(
  \sum_{i=1}^{n}w_i x_{k,i}+b
\right).
$$

AND je lineárně separovatelná: jediný neuron může po zavedení
rozhodovacího prahu rozlišit všechny čtyři kombinace. Sigmoida
zajišťuje hladký, diferencovatelný výstup, ale pro konečné
parametry nikdy nevrátí přesně $0$ ani $1$; MSE se proto může
blížit nule, aniž by jí přesně dosáhla. Ztrátu MSE volíme
pro návaznost na první experiment; binární křížovou entropii
(BCE) zatím nepoužijeme.

K trénování použijeme všechny čtyři binární kombinace,
tedy $N=4$. Runner připraví konstantní pole
`const float inputs[4][2]` a `const float targets[4]`; každá
dvojice vstupů tvoří jeden řádek a má odpovídající cíl.
Dávkové rozhraní předá obě pole a počet řádků bez kopírování.
Samostatnou validační množinu pro tuto úlohu nevytváříme.
Závěrečné vyhodnocení na stejných datech ukáže naučení
pravdivostní tabulky, nikoli zobecnění na
neviděné vstupy.

## 4. Architektura programu

Perceptron uchovává dvě váhy a bias, počítá sigmoidální
výstup a nezná optimalizátor. Optimizer drží nevlastněný
ukazatel na model, dva gradienty vah a gradient biasu;
vypočtené gradienty použije při aktualizaci parametrů.
Runner v souboru main.c má samostatnou funkci
`run_and_experiment`, kterou zavolá `main`.
V této funkci vytvoří na zásobníku nejprve Perceptron,
pak Optimizer, připraví konstantní trénovací pole,
řídí trénování a nakonec vyhodnotí celou tabulku AND.

```mermaid
classDiagram
    direction LR
    class Runner {
        +main()
        +run_and_experiment()
    }
    class Optimizer {
        +model
        +backward
        +gw
        +gb
    }
    class Perceptron {
        +w
        +b
        +perceptron()
        +perceptron_forward()
    }
    Runner ..> Perceptron : vytváří a vyhodnocuje
    Runner ..> Optimizer : volí funkci a řídí učení
    Optimizer ..> Perceptron : používá a upravuje
```

Optimizer drží ukazatel `backward` na dávkovou funkci.
Zvolená veřejná implementace spočítá hodnotu ztráty, její
derivaci i gradienty všech vah a biasu. Ztrátu vrátí;
gradienty uloží do instance Optimizer pro následnou aktualizaci.
První implementace používá MSE a BGD. Další ztrátová funkce
může používat stejné volání z Runneru bez změny Perceptronu.
Nepřímé volání proběhne jednou za dávku, nikoli pro každý vzorek.

Pro tento běh mají struktury přímo pole `float w[2]`
a `float gw[2]` a po jednom `float` pro bias a jeho
gradient. Velikost je daná při překladu; jiný počet vstupů
vyžaduje změnu velikosti polí a nový překlad. Konstrukce modelu
ani optimalizátoru nevyžaduje dynamickou alokaci.

```c
Perceptron perceptron(void);
float perceptron_forward(Perceptron *self, const float x[2]);
```

Konstruktor vrací strukturu hodnotou a náhodně inicializuje
obě váhy i bias pomocí `rand()` po vzoru experimentu 001.
Runner před jeho voláním nastaví `srand(42)`.
Stejné semínko nezaručuje stejnou posloupnost `rand()`
v různých C runtime. Funkce `perceptron_forward`
přijímá dvojici vstupů, vypočte
$z=w_1x_1+w_2x_2+b$ a vrátí $\sigma(z)$.

Jednotné volání `optimizer_backward` zajistí krátká
`static inline` funkce v hlavičce. Vyhodnotí ukazatel
na Optimizer pouze jednou a předá volání zvolené funkci
`backward`. Konstruktor modelu, výpočet výstupu,
konstruktor optimalizátoru a aktualizace gradientů navážou
na rozhraní experimentu 001. Dávka používá řádky po dvou
hodnotách, samostatné pole cílů a počet vzorků, například
`optimizer_backward(&training, inputs, targets, 4)`.
Podpis konstruktoru Optimizeru a rozsah náhodné
inicializace ještě určíme.

## 5. Gradient a řetězcové pravidlo

Pro diferencovatelnou ztrátu $L(\hat y,y)$ na jednom vzorku
platí

$$
\frac{\partial \hat y}{\partial z}
=\sigma'(z)
=\sigma(z)\bigl(1-\sigma(z)\bigr).
$$

Gradient vůči každé váze a biasu vznikne řetězením derivací:

$$
\frac{\partial L}{\partial w_i}
=\frac{\partial L}{\partial\hat y}
 \frac{\partial\hat y}{\partial z}
 \frac{\partial z}{\partial w_i}
=\frac{\partial L}{\partial\hat y}
 \sigma(z)\bigl(1-\sigma(z)\bigr)x_i,
\qquad i=1,\ldots,n,
$$

$$
\frac{\partial L}{\partial b}
=\frac{\partial L}{\partial\hat y}
 \sigma(z)\bigl(1-\sigma(z)\bigr).
$$

Pro čtvercovou ztrátu jednoho vzorku
$e_k=(\hat y_k-y_k)^2$ lze derivaci ztráty oddělit:

$$
\frac{\partial e_k}{\partial\hat y_k}
=2(\hat y_k-y_k).
$$

Optimizer ji pak řetězovým pravidlem násobí derivací
sigmoidy a příslušným vstupem. Pro zvolenou MSE trénovací
dávky se navíc sčítá přes vzorky a dělí jejich počtem:

$$
\frac{\partial\operatorname{MSE}}{\partial w_i}
=\frac{2}{N}\sum_{k=1}^{N}
(\hat y_k-y_k)\hat y_k(1-\hat y_k)x_{k,i},
\qquad i=1,\ldots,n,
$$

$$
\frac{\partial\operatorname{MSE}}{\partial b}
=\frac{2}{N}\sum_{k=1}^{N}
(\hat y_k-y_k)\hat y_k(1-\hat y_k).
$$

V blízkosti nasycení sigmoidy se její derivace blíží nule,
takže MSE může při nevhodném počátečním nastavení učit pomalu.
Zvolená funkce `backward` počítá ztrátu a průměrné
gradienty celé trénovací dávky. Výpočet derivace ztráty
může uvnitř funkce zůstat oddělený nebo se sloučit s dalšími
kroky, pokud to zlepší výkon či numerickou stabilitu.

## 6. Metodika a diagnostika

`main` zavolá `run_and_experiment`. Funkce
v každé epoše zavolá `optimizer_backward` nad celou
čtyřřádkovou dávkou, poté aktualizuje parametry (BGD).
První pozorovací běh má 10 epoch a rychlost učení
$\eta=0{,}01$. Stejně jako v experimentu 001 Runner
vypíše počáteční nastavení, pro každou epochu ztrátu před
aktualizací a váhy s biasem po ní. Po poslední epoše
vypíše pro každou kombinaci AND vstupy, očekávaný a
předpovězený výstup a nakonec MSE trénovací tabulky.
Samostatné běhy oddělí prázdným řádkem. Deset epoch
je počáteční pozorování, nikoli předpoklad dosažené přesnosti.

## 7. Otevřené návrhové otázky

Před implementací společně určíme rozsah inicializace,
konstruktor Optimizeru, klasifikační práh a měřitelné
požadavky experimentu.

## Slovník pojmů

- **Afinní funkce:** vztah $ax+b$; vážený součet vstupů
  představuje afinní část neuronu.
- **Perceptron:** v tomto experimentu model jediného neuronu se
  sigmoidální aktivací; původní perceptron používal prahovou aktivaci.
- **Aktivační funkce:** funkce použitá na afinní výstup neuronu;
  zde je zvolena sigmoida.
- **Sigmoida:** funkce $\sigma(z)$ s průběhem tvaru S při změně $z$.
- **Parametr:** váha $w_i$ nebo bias $b$ učený z dat.
- **Hyperparametr:** volba modelu nebo trénování, například
  aktivace či rychlost učení, která se neurčuje gradientem vah.
- **Řetězcové pravidlo:** způsob derivace složené funkce; zde
  propojuje derivaci ztráty, sigmoidy a váženého součtu.
- **MSE:** průměr čtverců rozdílů mezi výstupy neuronu a cíli
  vzorků v dané dávce.
- **BGD:** aktualizace parametrů jednou po výpočtu průměrného
  gradientu celé trénovací dávky.
- **Lineární separovatelnost:** možnost oddělit kombinaci
  $(1,1)$ od ostatních kombinací AND afinní rozhodovací hranicí.
- **Rozhodovací práh:** mez, která převede spojitý výstup
  sigmoidy na jednu ze dvou tříd.
- **Nasycení sigmoidy:** oblast výstupu blízko $0$ či $1$,
  kde je derivace aktivační funkce malá.
- **BCE:** binární křížová entropie, alternativní ztráta pro
  binární cíle, kterou zatím nepoužijeme.

# Specifikace experimentu lineární regrese

- **Projekt:** `001-LinearRegression`
- **Větev:** `experiment/001-linear-regression`
- **Stav dokumentu:** pracovní návrh; MSE a SGD jsou zvoleny jako výchozí směr, podrobnosti učení a akceptace čekají na společný brainstorming
- **Zdroj pravdy:** tento Markdown soubor
- **Datum:** 2026-11-24
- **Revize:** 2

## 1. Účel dokumentu

Experiment má ukázat, jak navrhnout a ověřit malý systém strojového učení v čistém C bez specializovaného frameworku. Model prokládá body v dvourozměrném grafu přímkou. Hlavním cílem první fáze je společně rozhodnout architekturu, způsob učení, ověřování a měření zdrojů před napsáním algoritmu.

Dokument zaznamenává potvrzené předpoklady zadané vlastníkem odděleně od návrhů a otevřených rozhodnutí. Vlastník navrhl oddělení modelu od optimalizátoru, MSE a stochastický gradientní sestup (SGD). Ještě nejsou určeny detaily trénovacího postupu ani jeho akceptační kritéria.

## 2. Rozsah a hranice

### 2.1 Potvrzený rozsah

- Vstupem dopředného výpočtu je skalár `x`, výstupem odhad `ŷ`.
- Model má dva parametry typu `float`: sklon `a` a posun `b`.
- Dopředný výpočet vychází ze vztahu `ŷ = x · a + b`; vztah `ŷ = x · a` je jeho případ s `b = 0`.
- Projekt používá C bez frameworku pro strojové učení a cílem je úsporný výpočet na dostupném hardwaru.
- Zdrojový kód, veřejné rozhraní, testy a platformní pravidla překladu budou oddělené podle kořenového `AGENTS.md`.

### 2.2 Zatím neurčený rozsah

Výchozí metodou učení je SGD se čtvercovou ztrátou; `a` i `b` se mají učit. Dosud nejsou určeny původ a reprezentace bodů, pořadí jejich zpracování, rychlost učení, počet průchodů, ukončení, diagnostika, přesnost a rozsah podporovaných vstupů. Není rozhodnuto o mini dávkách ani o paralelizaci. V této fázi se neimplementuje trénování ani dopředný výpočet.

## 3. Terminologie a jednotky

| Symbol | Význam | Jednotka |
|---|---|---|
| `x` | Vstupní souřadnice bodu. | Podle budoucích dat. |
| `y` | Pozorovaná výstupní souřadnice bodu. | Podle budoucích dat. |
| `ŷ` | Odhad výstupu vrácený modelem. | Stejná jako `y`. |
| `a` | Sklon přímky, váha násobící vstup. | Jednotka `y` dělená jednotkou `x`. |
| `b` | Posun přímky na výstupní ose. | Stejná jako `y`. |
| Reziduum | Rozdíl mezi pozorovaným a odhadnutým výstupem. | Stejná jako `y`. |

## 4. Teorie modelu a koncept učení

Přímka v této úloze převádí jeden vstup na jeden odhad:

```text
ŷ(x) = a · x + b
```

Pro `b = 0` prochází přímka počátkem souřadnic. Změna `a` mění její sklon, změna `b` ji posouvá. Při prokládání bodů je třeba určit, jak měřit rozdíl mezi `y` a `ŷ` a podle jakého pravidla měnit parametry. Tato dvě rozhodnutí nejsou vlastností samotné dopředné funkce.

Jde o afinní transformaci s identickou aktivací. Sigmoidový neuron používá tutéž transformaci před nelineární aktivací `σ(a · x + b)`; bez aktivace je výstupem přímo `a · x + b`. Myšlenka učení z gradientu se přenáší i na neuronové sítě, ale konkrétní derivace a chování více vrstev se po přidání aktivací mění.

Pro `N > 0` trénovacích bodů je zvoleným výchozím měřítkem střední čtvercová chyba:

```text
MSE = (1 / N) · Σᵢ (ŷᵢ - yᵢ)²
ŷᵢ = a · xᵢ + b
```

Pro SGD s jedním bodem lze použít čtvercovou ztrátu `eᵢ = (ŷᵢ - yᵢ)²`. Její gradient v aktuálních hodnotách parametrů je:

```text
g_a = ∂eᵢ/∂a = 2 · (ŷᵢ - yᵢ) · xᵢ
g_b = ∂eᵢ/∂b = 2 · (ŷᵢ - yᵢ)
a ← a - η · g_a
b ← b - η · g_b
```

`η > 0` je rychlost učení. Gradient vyjadřuje místní sklon ztráty v současném bodě `(a, b)`, nikoli přesný rozdíl ztrát mezi dvěma vzdálenými nastaveními parametrů. Krok v opačném směru obvykle snižuje ztrátu při vhodném `η`; příliš velký krok ji může zvýšit. Při SGD se aktualizuje po jednotlivých bodech. Gradient celé MSE je průměrem gradientů všech bodů; jeho použití pro jedinou aktualizaci by byl dávkový gradientní sestup. Analytické nejmenší čtverce zůstávají možnou kontrolní základnou, nikoli vybranou metodou trénování.

## 5. Požadavky na architekturu řešení

Zadání předpokládá tuto hodnotovou reprezentaci modelu:

```c
typedef struct {
    float a;
    float b;
} Regression;
```

Zamýšlené veřejné rozhraní obsahuje funkci `regression_forward`, která pro dané `x` vrací `self->a * x + self->b`. Struktura má být veřejná v `include/regression/regression.h`, zdrojový soubor v `src/` a jednotkové testy v `tests/`. Data, která se vejdou do automatické paměti, mají zůstat na zásobníku; případná dynamická paměť musí procházet `lib/alloc`.

V zadání se pro název typu střídá `Regresion` a `Regression`. Tento návrh používá `Regression` podle názvu dopředné funkce; pravopis a přesný podpis funkce včetně případného `const` ukazatele se potvrdí před vytvořením veřejné hlavičky.

Model bude v `src/regression.c` a veřejném `include/regression/regression.h`. Bude uchovávat `a`, `b` a počítat dopředný výstup, bez závislosti na optimalizátoru. `src/optimizer.c` a `include/regression/optimizer.h` budou znát veřejné rozhraní modelu a budou počítat ztrátu, gradienty a aktualizaci parametrů. Závislost je jednosměrná: `optimizer` → `regression`. Výpočet gradientu je specifický pro tento model, bez obecné vrstvy optimalizátorů. `a` a `b` jsou učitelné parametry modelu; rychlost učení `η` a případně počet průchodů jsou hyperparametry postupu učení. Přesné podpisy funkcí a vlastnictví trénovacích dat ještě nejsou určeny.

Rozdělení datových toků trénování zůstává otevřené do upřesnění SGD. Konfigurace `linux_x86_64` je výchozí. Pro Windows se připravují názvy `win32_x86_64` a jeho alias `win_32_x86_64`; oba mají používat stejná pravidla překladu. `Makefile` a platformní konfigurace zatím nesmějí hlásit úspěšný build nebo test, protože implementace neexistuje.

## 6. Výpočetní optimalizace a paralelizace

Optimalizace parametrů modelu řeší matematickou úlohu prokládání bodů. Optimalizace jejího výpočtu je samostatná technická úloha: volba rozložení bodů v paměti, průchodů daty, redukce součtů, vektorizace a případné rozdělení mezi vlákna. Před návrhem těchto změn se musí nejprve ustálit referenční sekvenční chování a měřítka přesnosti.

U jednoduchého modelu se dvěma parametry může režie obecného frameworku nebo paralelního plánování převážit samotný výpočet. To je hypotéza k měření, nikoli již dosažený výsledek. Každá optimalizace musí být porovnána se stejným problémem, daty, přesností a hardwarem.

## 7. Předběžné normativní požadavky

| ID | Požadavek | Stav |
|---|---|---|
| LR-001 | Model musí uchovávat `a` a `b` jako `float`. | Odvozeno přímo ze zadání. |
| LR-002 | Dopředná funkce musí vrátit `a · x + b`. | Odvozeno přímo ze zadání. |
| LR-003 | Varianta bez posunu musí být reprezentovatelná jako `b = 0`. | Odvozeno přímo ze zadání. |
| LR-004 | Produkční kód nesmí záviset na frameworku pro strojové učení. | Potvrzené pravidlo repozitáře. |
| LR-005 | Jednotkové testy musí ověřit dopřednou funkci a později zvolenou metodu učení. | Potvrzené pravidlo repozitáře. |
| LR-006 | Volba trénovacího algoritmu a akceptačních kritérií musí být schválena před jejich implementací. | Potvrzené pravidlo repozitáře. |
| LR-007 | Překlad musí podporovat `linux_x86_64` a Windows pod názvy `win32_x86_64` a `win_32_x86_64`; oba názvy Windows musí vést ke stejným pravidlům. | Požadavek na připravovanou implementaci. |
| LR-008 | Modul regrese nesmí záviset na optimalizátoru; optimalizátor používá veřejné rozhraní regrese. | Potvrzená architektura. |
| LR-009 | Ztráta pro `N > 0` bodů musí odpovídat MSE, tedy průměru druhých mocnin rozdílu `ŷ - y`. | Potvrzený výchozí směr; numerická smlouva se doplní. |
| LR-010 | SGD musí počítat gradient čtvercové ztráty vůči `a` a `b` a aktualizovat oba parametry v opačném směru. | Potvrzený výchozí směr; plán kroků se doplní. |

Požadavky na konvergenci, přesnost parametrů, chybná data a paměť budou doplněny po brainstormingu. Identifikátory zůstanou stabilní.

## 8. Akceptační scénáře a plán ověření

| ID | Požadavky | Vstup / podmínka | Očekávaný výsledek | Stav |
|---|---|---|---|---|
| LR-T01 | LR-001, LR-002 | `a = 2`, `b = 1`, `x = 3` | `ŷ = 7` | Plánováno; kód neexistuje. |
| LR-T02 | LR-002, LR-003 | `a = 2`, `b = 0`, `x = 3` | `ŷ = 6` | Plánováno; kód neexistuje. |
| LR-T03 | LR-002 | `a = -1`, `b = 2`, `x = -2` | `ŷ = 4` | Plánováno; kód neexistuje. |
| LR-T04 | LR-009 | Dva body s rozdíly `ŷ - y` rovnými `-1` a `-2` | `MSE = (1 + 4) / 2 = 2,5` | Plánováno; kód neexistuje. |
| LR-T05 | LR-010 | `a = b = 0`, `x = 2`, `y = 1`, `η = 0,1` | `g_a = -4`, `g_b = -2`; po jednom kroku `a = 0,4`, `b = 0,2` | Plánováno; kód neexistuje. |

Další testy učení, reprodukovatelnosti, hraničních hodnot a výkonu budou určeny po upřesnění postupu SGD a dat. `tests/` bude používat projektový adaptovaný `unit.c`/`unit.h` a `make test` bude vracet chybu při selhání. Pro podporovanou platformu se zaznamenají skutečné příkazy, hardware a výsledky. Po implementaci se ověří sestavení a jednotkové testy pro Linux a oba názvy Windows; samotné načtení konfiguračního souboru není ověřením překladu. Nyní nic z toho nelze vykazovat jako provedený test.

## 9. Rozhodnutí a otevřené otázky pro brainstorming

| ID | Otázka | Možnosti k diskusi |
|---|---|---|
| LR-OP-01 | Jaká je ztráta pro jeden krok SGD? | Rozhodnuto: `eᵢ = (ŷᵢ - yᵢ)²`, tedy MSE pro jeden bod. |
| LR-OP-02 | Jak řídit průchod SGD? | Pořadí bodů, pevná či proměnná rychlost učení, počet epoch, kritérium ukončení a případné mini dávky. |
| LR-OP-03 | Mají se učit oba parametry? | Rozhodnuto: `a` i `b`; `b = 0` lze použít jako počáteční hodnotu. |
| LR-OP-04 | Odkud přicházejí body a jak jsou uloženy? | Pevná testovací sada, pole předaná API, soubor či generátor. |
| LR-OP-05 | Jaký je přesný veřejný podpis a jméno typu? | Potvrdit `Regression` a rozhodnout o `const` u vstupního ukazatele. |
| LR-OP-06 | Jak posoudit výsledek? | Metriky, tolerance, degenerované vstupy, srovnávací základna a paměťové měření. |
| LR-OP-07 | Jak prezentovat proložení bodů? | Číselný výstup, export bodů a přímky pro graf nebo vlastní vizualizace. |

Oddělení modelu od optimalizátoru, MSE a SGD odpovídají nynějšímu rozhodnutí vlastníka. SGD je vhodné jako jednoduchý výchozí postup pro pozdější zkoumání učení neuronů; pro dvouparametrovou regresi lze jeho výsledek později porovnat s analytickým řešením. Jeho výběr zatím není důkazem lepší rychlosti ani paměťové úspory. Před implementací je třeba odsouhlasit otevřené detaily a akceptační kritéria.

## 10. Výsledky testování a závěr

Neexistuje implementace ani jednotkové testy tohoto experimentu. Žádný algoritmus učení, numerická přesnost, rychlost ani spotřeba paměti proto nebyly ověřeny. Závěr o použitelnosti pro prokládání bodů nelze zatím formulovat ani odsouhlasit.

## 11. Vazba na připravovanou implementaci a údržba

| Oblast | Plánovaná cesta |
|---|---|
| Veřejný model a dopředný výpočet | `include/regression/regression.h`, `src/regression.c` |
| Ztráta a učení parametrů | `include/regression/optimizer.h`, `src/optimizer.c` |
| Jednotkové testy | `tests/test_regression.c`, `tests/unit.c`, `tests/unit.h`, `tests/main.c` |
| Překlad pro výchozí platformu | `Makefile`, `config/linux_x86_64.mk` |
| Překlad pro Windows | `config/win32_x86_64.mk`, alias `config/win_32_x86_64.mk` |

Změna modelové rovnice, podpisu, metody učení, numerické smlouvy nebo akceptačních kritérií musí být nejprve promítnuta do této specifikace. Po schválení rozhodnutí se stav dokumentu upraví a začnou se implementovat odpovídající moduly a testy.

## 12. Vysvětlující slovník odborných pojmů

- **Lineární regrese:** Hledání přímky, která pro dané body popisuje vztah mezi jednou vstupní a jednou výstupní hodnotou.
- **Sklon (`a`):** Parametr určující, o kolik se odhad `ŷ` změní při jednotkové změně `x`.
- **Posun (`b`):** Hodnota odhadu v bodě `x = 0`; při nulovém posunu přímka prochází počátkem.
- **Dopředný výpočet:** Použití již zadaných parametrů modelu pro výpočet `ŷ` bez jejich změny.
- **Učení parametrů:** Postup, který z trénovacích bodů nastavuje `a` a případně `b` podle teprve zvoleného kritéria.
- **Ztrátová funkce:** Číselné měřítko rozdílu mezi pozorovanými a odhadnutými hodnotami, které má učení zmenšovat.
- **Reziduum:** Rozdíl `y - ŷ` pro jeden bod; jeho význam závisí na zvolené ztrátě.
- **Analytické nejmenší čtverce:** Výpočet parametrů ze souhrnných součtů bodů pro čtvercovou chybu za platných předpokladů.
- **Gradientní sestup:** Opakované posouvání parametrů proti gradientu ztráty; vyžaduje pravidlo kroku a ukončení.
- **SGD:** Stochastický gradientní sestup; zde aktualizace parametrů podle gradientu ztráty jednoho trénovacího bodu.
- **Gradient:** Vektor parciálních derivací ztráty podle parametrů `a` a `b` v jejich aktuálních hodnotách.
- **MSE:** Střední čtvercová chyba; průměr druhých mocnin rozdílů mezi odhadem a pozorováním.
- **Hyperparametr:** Hodnota určující postup učení, například rychlost učení `η`; není přímo součástí modelové přímky.
- **Sigmoidová aktivace:** Nelineární funkce aplikovaná na vážený součet neuronu; v této regresi se nepoužívá.
- **Identická aktivace:** Výstup se rovná vstupu aktivace, proto model vrací přímo `a · x + b`.
- **Referenční sekvenční chování:** Jednovláknový výsledek určený pro kontrolu budoucích výkonnostních změn.

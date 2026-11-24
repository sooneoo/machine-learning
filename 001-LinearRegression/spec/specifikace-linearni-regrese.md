# Specifikace experimentu lineární regrese

- **Projekt:** `001-LinearRegression`
- **Větev:** `experiment/001-linear-regression`
- **Stav dokumentu:** pracovní návrh; volba učení a akceptace čekají na společný brainstorming
- **Zdroj pravdy:** tento Markdown soubor
- **Datum:** 2026-11-24
- **Revize:** 0

## 1. Účel dokumentu

Experiment má ukázat, jak navrhnout a ověřit malý systém strojového učení v čistém C bez specializovaného frameworku. Model prokládá body v dvourozměrném grafu přímkou. Hlavním cílem první fáze je společně rozhodnout architekturu, způsob učení, ověřování a měření zdrojů před napsáním algoritmu.

Dokument zaznamenává potvrzené předpoklady zadané vlastníkem odděleně od návrhů a otevřených rozhodnutí. Žádný způsob optimalizace parametrů zatím není vybrán.

## 2. Rozsah a hranice

### 2.1 Potvrzený rozsah

- Vstupem dopředného výpočtu je skalár `x`, výstupem odhad `ŷ`.
- Model má dva parametry typu `float`: sklon `a` a posun `b`.
- Dopředný výpočet vychází ze vztahu `ŷ = x · a + b`; vztah `ŷ = x · a` je jeho případ s `b = 0`.
- Projekt používá C bez frameworku pro strojové učení a cílem je úsporný výpočet na dostupném hardwaru.
- Zdrojový kód, veřejné rozhraní, testy a platformní pravidla překladu budou oddělené podle kořenového `AGENTS.md`.

### 2.2 Zatím neurčený rozsah

Metoda učení, ztrátová funkce, původ a reprezentace trénovacích bodů, aktualizace `b`, diagnostika, přesnost vyhodnocení a rozsah podporovaných vstupů budou určeny při brainstormingu. Není rozhodnuto o dávkovém nebo průběžném zpracování ani o paralelizaci. V této fázi se neimplementuje trénování ani dopředný výpočet.

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

Možné ztráty zahrnují střední čtvercovou chybu a střední absolutní chybu. Možné postupy hledání parametrů zahrnují analytické nejmenší čtverce, dávkový gradientní sestup a jejich kombinaci pro vzájemnou kontrolu. Toto jsou **varianty k diskusi**, nikoli zvolený návrh. Jejich předpoklady, chování při odlehlých bodech a numerické meze musí být vyjasněny před implementací.

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

Rozdělení modulů a datových toků trénování zůstává otevřené do volby metody učení. `Makefile` a platformní konfigurace zatím nesmějí hlásit úspěšný build nebo test, protože implementace neexistuje.

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

Požadavky na ztrátu, konvergenci, přesnost parametrů, chybná data a paměť budou doplněny po brainstormingu. Identifikátory zůstanou stabilní.

## 8. Akceptační scénáře a plán ověření

| ID | Požadavky | Vstup / podmínka | Očekávaný výsledek | Stav |
|---|---|---|---|---|
| LR-T01 | LR-001, LR-002 | `a = 2`, `b = 1`, `x = 3` | `ŷ = 7` | Plánováno; kód neexistuje. |
| LR-T02 | LR-002, LR-003 | `a = 2`, `b = 0`, `x = 3` | `ŷ = 6` | Plánováno; kód neexistuje. |
| LR-T03 | LR-002 | `a = -1`, `b = 2`, `x = -2` | `ŷ = 4` | Plánováno; kód neexistuje. |

Testy učení, reprodukovatelnosti, hraničních hodnot a výkonu budou určeny až po výběru metody a dat. `tests/` bude používat projektový adaptovaný `unit.c`/`unit.h` a `make test` bude vracet chybu při selhání. Pro podporovanou platformu se zaznamenají skutečné příkazy, hardware a výsledky; nyní nic z toho nelze vykazovat jako provedený test.

## 9. Otevřená rozhodnutí pro brainstorming

| ID | Otázka | Možnosti k diskusi |
|---|---|---|
| LR-OP-01 | Která ztrátová funkce odpovídá cíli experimentu? | Čtvercová chyba, absolutní chyba nebo jiná zdůvodněná funkce. |
| LR-OP-02 | Jak se mají hledat `a` a `b`? | Analytický výpočet, dávkový gradientní sestup, obě metody nebo jiný postup. |
| LR-OP-03 | Má být `b` učený parametr, pevná nula, nebo podporovat oba režimy? | Porovnat dopad na model a testy. |
| LR-OP-04 | Odkud přicházejí body a jak jsou uloženy? | Pevná testovací sada, pole předaná API, soubor či generátor. |
| LR-OP-05 | Jaký je přesný veřejný podpis a jméno typu? | Potvrdit `Regression` a rozhodnout o `const` u vstupního ukazatele. |
| LR-OP-06 | Jak posoudit výsledek? | Metriky, tolerance, degenerované vstupy, srovnávací základna a paměťové měření. |
| LR-OP-07 | Jak prezentovat proložení bodů? | Číselný výstup, export bodů a přímky pro graf nebo vlastní vizualizace. |

Vybranou variantu, její důvod a zamítnuté alternativy zapíšeme sem po dohodě s vlastníkem repozitáře. Do té doby tento dokument není schválenou úplnou specifikací trénovacího algoritmu.

## 10. Výsledky testování a závěr

Neexistuje implementace ani jednotkové testy tohoto experimentu. Žádný algoritmus učení, numerická přesnost, rychlost ani spotřeba paměti proto nebyly ověřeny. Závěr o použitelnosti pro prokládání bodů nelze zatím formulovat ani odsouhlasit.

## 11. Vazba na připravovanou implementaci a údržba

| Oblast | Plánovaná cesta |
|---|---|
| Veřejný model a dopředný výpočet | `include/regression/regression.h`, `src/regression.c` |
| Jednotkové testy | `tests/test_regression.c`, `tests/unit.c`, `tests/unit.h`, `tests/main.c` |
| Překlad pro výchozí platformu | `Makefile`, `config/linux_x86_64.mk` |

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
- **Gradientní sestup:** Opakované posouvání parametrů podle směru změny ztráty; vyžaduje pravidlo kroku a ukončení.
- **Referenční sekvenční chování:** Jednovláknový výsledek určený pro kontrolu budoucích výkonnostních změn.

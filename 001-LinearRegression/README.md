# 001-LinearRegression

První experiment řady zaměřené na vlastní mechanismy strojového
učení v C bez frameworku. Cílem řady je co nejvyšší výkon a
přímá kontrola nad výpočtem; vedlejším cílem je porozumět teorii
i praxi. Tato fáze ověřuje model lineární regrese a optimalizaci
jeho parametrů; porovnání SGD, BGD a vlivu rychlosti učení je
dílčí výzkumnou otázkou. Výkon se zde zatím neměří.

`Regression` počítá $\hat y=ax+b$. `Optimizer` podporuje SGD
(aktualizaci po každém bodu) a BGD (aktualizaci po celé trénovací
množině). `src/main.c` porovnává oba postupy při
`lr = 0.01f` a `lr = 0.1f` během deseti epoch nad třemi body
zjednodušeného převodu kilometrů na míle. Navazující srovnání přidává
tři geometrie bodů, pevnou počáteční hodnotu a 100 epoch: přímku přes
počátek s učením jen sklonu, posunutou přímku s učením obou parametrů
a body mimo přímku. Dlouhý běh případu C porovnává stejné metody
po 1000 epochách, aby ověřil chování při nekolineárních datech.

Architektura, rovnice, akceptační scénáře, výsledky obou metod a
navržený závěr jsou v
[české specifikaci](spec/specifikace-linearni-regrese.md).

## Dokumentace zdrojového kódu

Veřejné rozhraní a důležité části implementace obsahují anglické
komentáře pro Doxygen. Konfigurace je v `Doxyfile` v kořeni projektu.
Dokumentaci lze na vyžádání vytvořit ručně z adresáře projektu:

```sh
doxygen Doxyfile
```

Výstup vznikne v ignorovaném adresáři `.cache/doxygen/`. Běžný
překlad, spuštění ani testy Doxygen nespouštějí.

## Překlad a spuštění

Linux x86-64:

```sh
make PLATFORM=linux_x86_64 test
make PLATFORM=linux_x86_64 exec
```

Windows x86-64 s MSYS2 MinGW-w64:

```powershell
mingw32-make PLATFORM=win32_x86_64 test
mingw32-make PLATFORM=win32_x86_64 exec
```

`test` používá sdílené `lib/unit`; produkční program tuto knihovnu
nelinkuje. Cesty k hlavičkám jsou v platformních konfiguracích přes
`LIB_ROOT`. Přesun knihoven vyžaduje změnu překladové cesty, nikoli
direktiv `#include`. Výstupy platforem se ukládají odděleně do
`.cache/<platform>/`.

Každý ze čtyř původních běhů nastavuje seed 42. SGD vypisuje ztrátu před
každým z 30 kroků, BGD MSE před každým z 10 kroků. SGD prochází
body v pevném pořadí. Model každého běhu se po tréninku ověří na
třech dosud neviděných bodech; vypíší predikce a validační MSE.
Dalších šest běhů vypisuje MSE celé sady po každé aktualizaci,
počet zvýšení MSE a konečnou validační MSE. Specifikace obsahuje
tabulky výsledků a grafy MSE po epochách včetně dlouhého běhu C.
Závěr experimentu je odsouhlasen a experiment je uzavřen.
